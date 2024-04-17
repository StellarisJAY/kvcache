#include "zset.h"
#include "skiplist.h"
#include "hashmap.h"
#include "str.h"
#include <stdlib.h>

void zset_insert(struct sorted_set *zset, struct str *key, double score)
{
    double *old = zset->dict->op.hash_get(zset->dict, key);
    if (old == NULL) {
        double *val = malloc(sizeof(double));
        *val = score;
        zset->dict->op.hash_put(zset->dict, key, val);
        zset->list->insert(zset->list, key, score);
    }
}

int zset_rank(struct sorted_set *zset, struct str *key)
{
    double *score = zset->dict->op.hash_get(zset->dict, key);
    if (score == NULL) return 0;
    return zset->list->rank(zset->list, key, *score);
}

struct sorted_set *create_zset()
{
    struct sorted_set *zset = malloc(sizeof(struct sorted_set));
    zset->dict = create_hashmap(compare_str, str_hash_func);
    zset->list = create_skiplist();
    zset->insert = zset_insert;
    zset->rank = zset_rank;
    return zset;
}