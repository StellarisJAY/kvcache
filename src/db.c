#include "db.h"
#include "str.h"
#include "lru.h"
#include "hashmap.h"
#include "list.h"
#include "zset.h"
#include "server.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REGISTER_COMMAND(db, cmd, handler) \
    db->handlers->op.hash_put(db->handlers, from_char_array(cmd, strlen(cmd)), handler)

int db_get_str(struct database   *db, 
               struct connection *conn,  
               int                argc, 
               struct resp_cmd   *argv, 
               struct resp_cmd   *response)
{
    struct lru_map *dict = db->get_db(db, conn->selected_db);
    if (argc == 0) {
        response->type = ERROR;
        response->data = ERR_WRONG_ARGUMENT_NUM;
        return 0;
    }
    struct str *key;
    if (argc == 1) {
        key = argv[0].data;
        struct db_entry *entry = dict->op.get(dict, key);
        if (entry == NULL) {
            nil_response(response);
            return 0;
        }
        if (entry->type == RAW) {
            struct str *bulk = copy_str(entry->data);
            create_bulk_response(bulk, response);
            return 0;
        }else {
            response->type = ERROR;
            response->data = ERR_WRONG_TYPE;
            return 0;
        }
    }

    struct resp_cmd_array *arr = malloc(sizeof(struct resp_cmd_array));
    arr->data = malloc(sizeof(struct resp_cmd) * argc);
    arr->n = argc;
    for (int i = 0; i < argc; i++) {
        key = argv[i].data;
        struct db_entry *entry = dict->op.get(dict, key);
        if (entry == NULL) {
            arr->data[i].type = NIL;
            continue;
        }
        if (entry->type == RAW) {
            struct str *bulk = copy_str(entry->data);
            create_bulk_response(bulk, &arr->data[i]);
        }else {
            response->type = ERROR;
            response->data = ERR_WRONG_TYPE;
            free(arr->data);
            free(arr);
        }
    }
    create_array_response(arr, response);
}

int db_set_str(struct database   *db, 
               struct connection *conn,  
               int                argc, 
               struct resp_cmd   *argv, 
               struct resp_cmd   *response)
{
    struct lru_map *dict = db->get_db(db, conn->selected_db);
    if (argc < 2) {
        response->type = ERROR;
        response->data = ERR_WRONG_ARGUMENT_NUM;
        return 0;
    }
    struct str *key = copy_str(argv[0].data);
    struct str *val = copy_str(argv[1].data);
    struct db_entry *entry = dict->op.get(dict, key);
    if (entry == NULL) {
        db->put_entry(db, conn->selected_db, key, RAW, val);
    }else {
        free_db_entry(entry);
        free(key->buf);
        free(key);
        entry->data = val;
        entry->type = RAW;
    }
    ok_response(response);
    return 0;
}

int db_del(struct database   *db, 
           struct connection *conn,  
           int                argc, 
           struct resp_cmd   *argv, 
           struct resp_cmd   *response)
{
    struct lru_map *dict = db->get_db(db, conn->selected_db);
    if (argc == 0) {
        response->type = ERROR;
        response->data = ERR_WRONG_ARGUMENT_NUM;
        return 0;
    }
    int res = 0;
    for (int i = 0; i < argc; i++) {
        struct str *key = argv[i].data;
        struct db_entry *entry = dict->op.get(dict, key);
        if (entry == NULL) {
            continue;
        }else {
            dict->op.del(dict, key);
            free_db_entry(entry);
            free(entry);
            res += 1;
        }
    }
    create_int_response(res, response);
    return  0;
}

int db_strlen(struct database *db, 
              struct connection *conn, 
              int argc, 
              struct resp_cmd *argv, 
              struct resp_cmd *response)
{
    struct lru_map *dict = db->get_db(db, conn->selected_db);
    if (argc != 1) {
        response->type = ERROR;
        response->data = ERR_WRONG_ARGUMENT_NUM;
        return 0;
    }
    struct str *key = argv[0].data;
    struct db_entry *entry = dict->op.get(dict, key);
    if (entry == NULL) {
        create_int_response(0, response);
        return 0;
    }
    if (entry->type != RAW) {
        response->type = ERROR;
        response->data = ERR_WRONG_TYPE;
        return 0;
    }
    int length = ((struct str*)entry->data)->length;
    create_int_response(length, response);
    return 0;
}

int db_exists(struct database *db, 
              struct connection *conn, 
              int argc, 
              struct resp_cmd *argv, 
              struct resp_cmd *response)
{
    struct lru_map *dict = db->get_db(db, conn->selected_db);
    if (argc == 0) {
        response->type = ERROR;
        response->data = ERR_WRONG_ARGUMENT_NUM;
        return 0;
    }
    struct str *key;
    int res = 0;
    for (int i = 0; i < argc; i++) {
        key = argv[i].data;
        if (dict->op.get(dict, key)) {
            res++;
        }
    }
    create_int_response(res, response);
    return 0;
}

void *db_get_entry(struct database *db, 
                   int idx, 
                   struct str *key, 
                   enum db_entry_type type)
{
    struct lru_map *m = db->get_db(db, idx);
    if (m == NULL) return NULL;
    struct db_entry *entry = m->op.get(m, key);
    if (entry == NULL || entry->type != type) return NULL;
    return entry->data;
}

struct lru_map *db_get_database(struct database *db, int idx)
{
    if (idx < 0 || idx >= MAX_DB) return NULL;
    return db->maps[idx];
}

struct hashmap *db_get_hash(struct database *db, 
                            int idx, 
                            struct str *key)
{
    
    return db_get_entry(db, idx, key, HASH);
}

struct link_list *db_get_list(struct database *db, 
                              int idx, 
                              struct str *key)
{
    
    return db_get_entry(db, idx, key, LIST);
}

struct sorted_set *db_get_zset(struct database *db, 
                               int idx, 
                               struct str *key)
{
    
    return db_get_entry(db, idx, key, ZSET);
}

void db_put_entry(struct database    *db, 
                  int                 idx, 
                  struct str         *key, 
                  enum db_entry_type  type, 
                  void               *value)
{
    struct lru_map *m = db->get_db(db, idx);
    if (m == NULL) return;
    struct db_entry *entry = m->op.get(m, key);
    if (entry != NULL) return;
    entry = malloc(sizeof(struct db_entry));
    entry->type = type;
    entry->data = value;
    m->op.put(m, key, entry);
}

int db_handle_command(struct database   *db, 
                      struct connection *conn, 
                      struct resp_cmd   *request, 
                      struct resp_cmd   *response)
{
    // get argc and argv
    struct resp_cmd_array *array = request->data;
    int argc = array->n-1;
    struct resp_cmd *argv = &array->data[1];
    struct str *name;
    if (array->data[0].type == BULK_STRING) {
        name = array->data[0].data;
    }else {
        goto UNKNOWN_COMMAND;
    }
    str_to_upper(name);
    command_handler handler = db->handlers->op.hash_get(db->handlers, name);
    if (handler == NULL) {
        goto UNKNOWN_COMMAND;
    }
    return handler(db, conn, argc, argv, response);
UNKNOWN_COMMAND:
    // todo create error response
    create_int_response(-1, response);
    return 0;
}

struct database *create_database()
{
    struct database *db = malloc(sizeof(struct database));
    for (int i = 0; i < MAX_DB; i++) {
        // todo calculate capacity
        db->maps[i] = create_lru_map(1024, compare_str, str_hash_func);
    }
    db->handlers = create_hashmap(compare_str, str_hash_func);

    REGISTER_COMMAND(db, "SET", db_set_str);
    REGISTER_COMMAND(db, "GET", db_get_str);
    REGISTER_COMMAND(db, "EXISTS", db_exists);
    REGISTER_COMMAND(db, "DEL", db_del);
    REGISTER_COMMAND(db, "STRLEN", db_strlen);
    
    db->get_db = db_get_database;
    db->get_list = db_get_list;
    db->get_hash = db_get_hash;
    db->put_entry = db_put_entry;
    db->handle_command = db_handle_command;
    return db;
}

void free_db_entry(struct db_entry *entry)
{
    switch (entry->type) {
    case RAW:
        struct str *s = entry->data;
        free(s->buf);
        free(s);
        break;
    // todo free other datastructures
    case LIST:
    case HASH:
    case ZSET:
    }
}