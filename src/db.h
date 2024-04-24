#ifndef DB_H
#define DB_H
#define MAX_DB 8
#include "lru.h"
#include "str.h"
#include "hashmap.h"
#include "list.h"
#include "zset.h"
#include "protocol.h"
#include "server.h"

enum db_entry_type {
    RAW,
    LIST,
    HASH,
    SET,
    ZSET,
};

struct db_entry 
{
    enum db_entry_type type;
    void *data;
};

struct database
{
    struct lru_map *maps[MAX_DB];
    struct hashmap *handlers;

    struct lru_map *(*get_db)(struct database *db, int idx);

    struct hashmap *(*get_hash)(struct database *db, int idx, struct str *key);
    struct link_list *(*get_list)(struct database *db, int idx, struct str *key);
    struct sorted_set *(*get_zset)(struct database *db, int idx, struct str *key);
    void (*put_entry)(struct database *db, int idx, struct str *key, enum db_entry_type type, void *value);
    int (*handle_command)(struct database *db, struct connection *conn, struct resp_cmd *request, struct resp_cmd *response);
};

typedef int (*command_handler)(struct database *db, struct connection *conn, int argc, struct resp_cmd *argv, struct resp_cmd *response);

struct database *create_database();

#endif