#ifndef ZSET_H
#define ZSET_H
#include "skiplist.h"
#include "hashmap.h"
#include "str.h"
struct sorted_set 
{
    struct hashmap *dict;
    struct skiplist *list;

    void (*insert)(struct sorted_set *zset, struct str *key, double score);
    int (*rank)(struct sorted_set *zset, struct str *key);
};

struct sorted_set *create_zset();
#endif