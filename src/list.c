#include "list.h"
#include <stdlib.h>
#include "str.h"

void list_free(struct link_list *list)
{

}

void list_add_head(struct link_list *list, struct str *value)
{
    struct list_node *node = malloc(sizeof(struct list_node));
    node->value = value;
    if (list->size == 0) {
        list->head = list->tail = node;
        list->size++;
    }else {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
}

void list_add_tail(struct link_list *list, struct str *value)
{
    struct list_node *node = malloc(sizeof(struct list_node));
    node->value = value;
    if (list->size == 0) {
        list->head = list->tail = node;
        list->size++;
    }else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
}

struct str *list_del_head(struct link_list *list)
{
    if (list->size == 0) {
        return NULL;
    } else {
        struct list_node *node = list->head;
        list->head = node->next;
        if (list->head) list->head->prev = NULL;
        node->next = NULL;
        struct str *res = node->value;
        free(node);
        list->size--;
        return res;
    }
}

struct str *list_del_tail(struct link_list *list)
{
    if (list->size == 0) {
        return NULL;
    } else {
        struct list_node *node = list->tail;
        list->tail = node->prev;
        if (list->tail) list->tail->next = NULL;
        node->prev = NULL;
        struct str *res = node->value;
        free(node);
        return res;
    }
}

struct str *list_get(struct link_list *list, int get_head)
{
    if (get_head) return list->head;
    else return list->tail;
}

void list_foreach(struct link_list *list, int (*cosumer)(struct list_node *node))
{
    list_for_each(list) {
        if (!cosumer(cur)) break;
    }
}

struct link_list *create_list()
{
    struct link_list *list = malloc(sizeof(struct link_list));
    list->head = list->tail = NULL;
    list->add_head = list_add_head;
    list->add_tail = list_add_tail;
    list->free = list_free;
    list->del_head = list_del_head;
    list->del_tail = list_del_tail;
    list->get = list_get;
    list->foreach = list_foreach;
    return list;
}