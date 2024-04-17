#ifndef LIST_H
#define LIST_H
#include "str.h"
struct list_node
{
    struct list_node *prev;
    struct list_node *next;
    struct str *value;
};

struct link_list
{
    struct list_node *head;
    struct list_node *tail;
    unsigned int size;

    void (*free)(struct link_list*);
    void (*add_head)(struct link_list*, struct str*);
    void (*add_tail)(struct link_list*, struct str*);
    struct str *(*get)(struct link_list*, int);
    struct str *(*del_head)(struct link_list*);
    struct str *(*del_tail)(struct link_list*);
    void (*foreach)(struct link_list *list, int (*cosumer)(struct list_node *node));
};

struct link_list *create_list();

#define list_for_each(list) \
    for(struct list_node *cur = list->head; cur != NULL; cur = cur->next)

#endif