#include "db.h"
#include "str.h"
#include "lru.h"
#include "hashmap.h"
#include "list.h"
#include "zset.h"
#include "server.h"
#include <stdlib.h>
#include <stdio.h>

int db_get_str(struct database *db, 
               struct connection *conn,  
               int argc, 
               struct resp_cmd *argv, 
               struct resp_cmd *response)
{
    create_int_response(1, response);
    return 0;
}

int db_set_str(struct database *db, 
               struct connection *conn,  
               int argc, 
               struct resp_cmd *argv, 
               struct resp_cmd *response)
{
    create_int_response(1, response);
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

void db_put_entry(struct database *db, 
                  int idx, 
                  struct str *key, 
                  enum db_entry_type type, 
                  void *value)
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

int db_handle_command(struct database *db, 
                      struct connection *conn, 
                      struct resp_cmd *request, 
                      struct resp_cmd *response)
{
    // get argc and argv
    struct resp_cmd_array *array = request->data;
    int argc = array->n-1;
    struct resp_cmd *argv = &array->data[1];
    if (array->data[0].type != BULK_STRING) {
        goto UNKNOWN_COMMAND;
    }
    // find command handler
    struct str *name = array->data[0].data;
    command_handler handler = db->handlers->op.hash_get(db->handlers, name);
    if (handler == NULL) {
        goto UNKNOWN_COMMAND;
    }
    return handler(db, conn, argc, argv, response);
UNKNOWN_COMMAND:
    // todo create error response
    create_int_response(2, response);
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
    db->handlers->op.hash_put(db->handlers, from_char_array("SET", 3), db_set_str);
    db->handlers->op.hash_put(db->handlers, from_char_array("GET", 3), db_get_str);

    db->get_db = db_get_database;
    db->get_list = db_get_list;
    db->get_hash = db_get_hash;
    db->put_entry = db_put_entry;
    db->handle_command = db_handle_command;
    return db;
}