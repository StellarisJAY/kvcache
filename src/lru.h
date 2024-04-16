#ifndef LRU_H
#define LRU_H
#include "hashmap.h"

#define lru_for_each(cur,lru) \
    for(struct lru_entry *cur=lru->head->next; cur != lru->tail; cur=cur->next)

struct lru_entry {
    struct lru_entry *prev;
    struct lru_entry *next;
    void *value;
    void *key;
    unsigned int keyLen;

    struct lru_entry_op {
        void (*remove)(struct lru_entry*);
        void (*add_next)(struct lru_entry*, struct lru_entry*);
        void (*add_prev)(struct lru_entry*, struct lru_entry*);
    } op;
};

struct lru_map {
    struct hashmap *hmap;
    struct lru_entry *head;
    struct lru_entry *tail;

    unsigned int cap;
    unsigned int size;

    struct lru_map_op {
        void (*put)(struct lru_map *self, void *key, unsigned int keyLen, void *val);
        void (*del)(struct lru_map *self, void *key, unsigned int keyLen);
        void *(*get)(struct lru_map *self, void *key, unsigned int keyLen);
        void (*free)(struct lru_map *self);
    } op;
};

struct lru_map *create_lru_map(unsigned int capacity, int (*compare)(void*, void*), unsigned long long (*hash_func)(void*, unsigned int));
#endif