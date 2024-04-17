#include "hashmap.h"
#include <stdlib.h>
#include "highwayhash.h"
#include <stdio.h>

void hash_put(struct hashmap *hmap, void *key, void *val) 
{
    unsigned long long hash = hmap->op.hash_func(key);
    struct bucket *b = &hmap->bmap[hash % hmap->bmap_size];
    long hash_high = hash >> 32;
    for (; b != NULL; b = b->overflow) {
        for (int i = 0; i < BUCKET_SIZE; i++) {
            if (b->tophash[i] == 0) {
                b->tophash[i] = hash_high;
                b->keys[i] = key;
                b->vals[i] = val;
                hmap->size++;
                return;    
            }
            if (b->tophash[i] != hash_high) continue;
            if (b->keys[i] != key) continue;
            b->vals[i] = val;
            return;
        }
    }
    struct bucket *overflow = malloc(sizeof(struct bucket));
    b->overflow = overflow;
    overflow->keys[0] = key;
    overflow->tophash[0] = hash_high;
    overflow->vals[0] = val;
    hmap->size++;
}

void *hash_get(struct hashmap *hmap, void *key)
{
    unsigned long long hash = hmap->op.hash_func(key);
    struct bucket *b = &hmap->bmap[hash % hmap->bmap_size];
    long hash_high = hash >> 32;
    for (; b != NULL; b = b->overflow){
        for (int i = 0; i < BUCKET_SIZE; i++) {
            if (b->tophash[i] != hash_high) continue;
            if (hmap->compare(key, b->keys[i]) != 0) continue;
            return b->vals[i];
        }
    }
    return NULL;
}

void *hash_del(struct hashmap *hmap, void *key)
{
    unsigned long long hash = hmap->op.hash_func(key);
    struct bucket *b = &hmap->bmap[hash % hmap->bmap_size];
    long hash_high = hash >> 32;
    for (; b != NULL; b = b->overflow) {
        for (int i = 0; i < BUCKET_SIZE; i++) {
            if (b->tophash[i] != hash_high) continue;
            if (hmap->compare(key, b->keys[i]) != 0) continue;
            b->tophash[i] = 0;
            b->vals[i] = NULL;
            b->keys[i] = NULL;
            hmap->size--;
        }
    }
}

void hash_free(struct hashmap *hmap)
{
    for (int i = 0; i < hmap->bmap_size; i++) {
        struct bucket *b = &hmap->bmap[i];
        while(b != NULL) {
            for (int j = 0; j < BUCKET_SIZE; j++) {
                free(b->vals[i]);
            }
            struct bucket *overflow = b->overflow;
            free(b);
            b = overflow;
        }
    }
    free(hmap->bmap);
    free(hmap);
}


struct hashmap *create_hashmap(int (*compare)(void*, void*), unsigned long long (*hash_func)(void *))
{
    struct hashmap *hmap = malloc(sizeof(struct hashmap));
    hmap->bmap_size = 16;
    hmap->bmap = malloc(sizeof(struct bucket) * hmap->bmap_size);
    struct hashmap_op op = {
        .hash_get = hash_get,
        .hash_put = hash_put,
        .hash_del = hash_del,
        .hash_func = hash_func,
        .free = hash_free,
    };
    hmap->op = op;
    hmap->compare = compare;
    return hmap;
}

