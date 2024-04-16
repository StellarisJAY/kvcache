#ifndef DB_H
#define DB_H
#define MAX_DB 8
#include "lru.h"
#include "str.h"
#include "hashmap.h"

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

    struct lru_map *(*get_db)(struct database *db, int idx);
    struct str *(*get_str)(struct database *db, int idx, struct str *key);
    int (*set_str)(struct database *db, int idx, struct str *key, struct str *value);

    struct hashmap *(*get_hash)(struct database *db, int idx, struct str *key);
};

struct database *create_database();

#endif