#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "str.h"

#define MAX_LEVELS 8

struct skiplist_node
{
    struct str *key;
    double score;
    struct skiplist_node *prev;
    struct skiplist_node_level
    {
        struct skiplist_node *next;
        unsigned int span;
    } levels[MAX_LEVELS];
};

struct skiplist
{
    int levels;
    struct skiplist_node *head;
    int size;

    void (*insert)(struct skiplist *s, struct str *key, double score);
    int (*rank)(struct skiplist *s, struct str *key, double score);
    void (*remove)(struct skiplist *s, struct str *key, double score);
    #ifdef DEBUG
    void (*print_skiplist)(struct skiplist *s);
    #endif
};

struct skiplist *create_skiplist();

#endif