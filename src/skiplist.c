#include "skiplist.h"
#include "str.h"
#include <stdlib.h>


int random_levels()
{
    // todo better random levels
    return rand() % MAX_LEVELS;
}

//0 ------------> 5 ---------------->10
//0 ----->2-----> 5 ------->7------->10
//0 ----->2-----> 5 --->6-->7------->10 
//0 -->1->2-----> 5 --->6-->7------->10
//0 -->1->2-->4-> 5 --->6-->7------->10
void skiplist_insert(struct skiplist *s, struct str *key, double score)
{
    struct skiplist_node *node = malloc(sizeof(struct skiplist_node));
    node->key = key;
    node->score = score;
    for (int i = 0; i < MAX_LEVELS; i++) node->levels[i].next = NULL;
    struct skiplist_node *cur = s->head;
    struct skiplist_node *path[MAX_LEVELS];
    for(int level = MAX_LEVELS - 1; level >= 0; level--) {
        while(cur != NULL) {
            struct skiplist_node *next = cur->levels[level].next;
            if (next != NULL && ((next->score < score) || (next->score==score && compare_str(next->key, key) < 0))) {
                cur = next;
            }else {
                break;
            }
        }
        path[level] = cur;
    }
    int levels = random_levels();
    for (int i = 0; i < levels; i++) {
        node->levels[i].next = path[i]->levels[i].next;
        path[i]->levels[i].next = node;
    }

    // todo update spans
    s->size++;
}

#ifdef DEBUG
#include <stdio.h>
void print_skiplist(struct skiplist *s)
{
    for (int i = MAX_LEVELS - 1; i >= 0; i--) {
        struct skiplist_node *node = s->head->levels[i].next;
        while(node != NULL) {
            printf("%f", node->score);
            node = node->levels[i].next;
        }
        printf("\n");
    }
}
#endif

struct skiplist *create_skiplist()
{
    struct skiplist *s = malloc(sizeof(struct skiplist));
    s->levels = MAX_LEVELS;
    s->insert = skiplist_insert;
    #ifdef DEBUG
    s->print_skiplist = print_skiplist;
    #endif

    s->head = malloc(sizeof(struct skiplist_node));
    s->head->prev = NULL;
    for (int i = 0; i < MAX_LEVELS; i++) {
        s->head->levels[i].span = 0;
        s->head->levels[i].next = NULL;
    }
    return s;
}