#include "db.h"
#include "str.h"
#include "lru.h"
#include "hashmap.h"
#include "list.h"
#include <stdlib.h>
#include <stdio.h>

int db_set_str(struct database *db, int idx, struct str *key, struct str *value)
{   
    struct lru_map *m = db->get_db(db, idx);
    if (m == NULL) return 0;
    struct db_entry *entry = malloc(sizeof(struct db_entry));
    entry->type = RAW;
    entry->data = value;
    m->op.put(m, key, sizeof(struct str), entry);
    return 1;
}

struct str *db_get_str(struct database *db, int idx, struct str *key)
{
    struct lru_map *m = db->get_db(db, idx);
    if (m == NULL) return NULL;
    struct db_entry *entry = m->op.get(m, key, sizeof(struct str));
    if (entry == NULL || entry->type != RAW) return NULL;
    return entry->data;
}

struct lru_map *db_get_database(struct database *db, int idx)
{
    if (idx < 0 || idx >= MAX_DB) return NULL;
    return db->maps[idx];
}

struct hashmap *db_get_hash(struct database *db, int idx, struct str *key)
{
    struct lru_map *m = db->get_db(db, idx);
    if (m == NULL) return NULL;
    struct db_entry *entry = m->op.get(m, key, sizeof(struct str));
    if (entry == NULL || entry->type != HASH) return NULL;
    return entry->data;
}

struct link_list *db_get_list(struct database *db, int idx, struct str *key)
{
    struct lru_map *m = db->get_db(db, idx);
    if (m == NULL) return NULL;
    struct db_entry *entry = m->op.get(m, key, sizeof(struct str));
    if (entry == NULL || entry->type != LIST) return NULL;
    return entry->data;
}

void db_put_entry(struct database *db, int idx, struct str *key, enum db_entry_type type, void *value)
{
    struct lru_map *m = db->get_db(db, idx);
    if (m == NULL) return;
    struct db_entry *entry = m->op.get(m, key, sizeof(struct str));
    if (entry != NULL) return;
    entry = malloc(sizeof(struct db_entry));
    entry->type = type;
    entry->data = value;
    m->op.put(m, key, sizeof(struct str), entry);
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
    db->get_list = db_get_list;
    db->get_hash = db_get_hash;
    db->put_entry = db_put_entry;
    return db;
}