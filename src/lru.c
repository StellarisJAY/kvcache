#include "lru.h"
#include "hashmap.h"
#include <stdlib.h>
#include <stdio.h>

void move_to_tail(struct lru_map *self, struct lru_entry *entry)
{
    entry->op.remove(entry);
    self->tail->op.add_prev(self->tail, entry);
}

void lru_entry_remove(struct lru_entry *self)
{
    if (self->next != NULL) {
        self->next->prev = self->prev;
    }
    if (self->prev != NULL) {
        self->prev->next = self->next;
    }

    self->prev = NULL;
    self->next = NULL;
}

void lru_entry_add_next(struct lru_entry *self, struct lru_entry *next)
{
    self->next->prev = next;
    next->next = self->next;
    self->next = next;
    next->prev = self;
}

void lru_entry_add_prev(struct lru_entry *self, struct lru_entry *prev)
{
    self->prev->next = prev;
    prev->prev = self->prev;
    self->prev = prev;
    prev->next = self;
}

struct lru_entry *create_lru_entry(void *key, unsigned int keyLen, void *val)
{
    struct lru_entry *entry = malloc(sizeof(struct lru_entry));
    struct lru_entry_op op = {
        .add_next = lru_entry_add_next,
        .remove = lru_entry_remove,
        .add_prev = lru_entry_add_prev,
    };
    entry->op = op;
    entry->key = key;
    entry->value = val;
    entry->keyLen = keyLen;
    return entry;
}

void lru_map_put(struct lru_map *self, void *key, unsigned int keyLen, void *val)
{
    struct lru_entry *old = self->hmap->op.hash_get(self->hmap, key, keyLen);
    if (old != NULL) {
        old->value = val;
        move_to_tail(self, old);
    } else {
        if (self->size == self->cap) {
            struct lru_entry *evict = self->head->next;
            evict->op.remove(evict);
            self->hmap->op.hash_del(self->hmap, evict->key, evict->keyLen);
            self->size--;
            free(evict);
        }
        struct lru_entry *entry = create_lru_entry(key, keyLen, val);
        self->tail->op.add_prev(self->tail, entry);
        self->size++;
        self->hmap->op.hash_put(self->hmap, key, keyLen, entry);
    }
}

void lru_map_del(struct lru_map *self, void *key, unsigned int keyLen)
{
    struct lru_entry *entry = self->hmap->op.hash_del(self->hmap, key, keyLen);
    if (entry == NULL) {
        return;
    }
    entry->op.remove(entry);
    free(entry->value);
    free(entry);
}

void *lru_map_get(struct lru_map *self, void *key, unsigned int keyLen)
{
    struct lru_entry *entry = self->hmap->op.hash_get(self->hmap, key, keyLen);
    if (entry == NULL) {
        return NULL;
    }
    move_to_tail(self, entry);
    return entry->value;
}

void lru_map_free(struct lru_map *self)
{
    self->hmap->op.free(self->hmap);
    free(self);
}

struct lru_map *create_lru_map(unsigned int capacity, int (*compare)(void*, void*), unsigned long long (*hash_func)(void*, unsigned int))
{
    struct lru_map *lru = malloc(sizeof(struct lru_map));
    struct lru_map_op op = {
        .del = lru_map_del,
        .put = lru_map_put,
        .get = lru_map_get,
        .free = lru_map_free,
    };
    lru->op = op;
    lru->cap = capacity;
    lru->hmap = create_hashmap(compare, hash_func);
    lru->head = create_lru_entry(NULL, 0, NULL);
    lru->tail = create_lru_entry(NULL, 0, NULL);

    lru->head->next = lru->tail;
    lru->tail->prev = lru->head;
    return lru;
}