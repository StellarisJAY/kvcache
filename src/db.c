#include "db.h"
#include "str.h"
#include "lru.h"
#include <stdlib.h>
#include <stdio.h>

int db_set_str(struct database *db, int idx, struct str *key, struct str *value)
{   
    
    return 0;
}

struct str *db_get_str(struct database *db, int idx, struct str *key)
{
    return NULL;
}

struct lru_map *db_get_database(struct database *db, int idx)
{
    return NULL;
}

struct database *create_database()
{
    struct database *db = malloc(sizeof(struct database));
    for (int i = 0; i < MAX_DB; i++) {
        // todo calculate capacity
        db->maps[i] = create_lru_map(1024, compare_str, str_hash_func);
    }
    db->get_db = db_get_database;
    db->get_str = db_get_str;
    db->set_str = db_set_str;
    return db;
}