#include "skiplist.h"
#include "str.h"
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define can_move_forward(next, score, key) \
    next != NULL && ((next->score < score) || (next->score==score && compare_str(key, next->key) < 0))

int random_levels()
{
    // todo better random levels
    return (rand() % MAX_LEVELS) + 1;
}

//0 ---------------> 5 ---------------->10
//0 -0.5---->2-----> 5 ------->7------->10
//0 -0.5---->2-----> 5 --->6-->7------->10 
//0 -0.5->1->2-----> 5 --->6-->7------->10
//0 -0.5->1->2-->4-> 5 --->6-->7------->10
void skiplist_insert(struct skiplist *s, struct str *key, double score)
{
    struct skiplist_node *node = malloc(sizeof(struct skiplist_node));
    node->key = key;
    node->score = score;
    memset(node->levels, 0, sizeof(struct skiplist_node_level) * MAX_LEVELS);

    struct skiplist_node *cur = s->head;
    struct skiplist_node *path[MAX_LEVELS];
    int ranks[MAX_LEVELS];
    memset(ranks, 0, MAX_LEVELS * sizeof(int));

    for(int i = MAX_LEVELS - 1; i >= 0; i--) {
        while(cur != NULL) {
            struct skiplist_node *next = cur->levels[i].next;
            if (can_move_forward(next, score, key)) {
                cur = next;
                ranks[i] += cur->levels[i].span;
            }else {
                break;
            }
        }
        path[i] = cur;
    }

    int levels = random_levels();
    for (int i = 0; i < levels; i++) {
        node->levels[i].next = path[i]->levels[i].next;
        path[i]->levels[i].next = node;
        node->levels[i].span = path[i]->levels[i].span - (ranks[0] - ranks[1]);
        path[i]->levels[i].span = ranks[i] + 1;
    }
    for (int i = levels; i < MAX_LEVELS; i++) {
        path[i]->levels[i].span += 1;
    }
    // todo update spans
    s->size++;
}

void skiplist_remove(struct skiplist *s, struct str *key, double score)
{
    // todo skiplist remove
}

int skiplist_rank(struct skiplist *s, struct str *key, double score)
{
    struct skiplist_node *cur = s->head;
    int rank = 0;
    for (int i = MAX_LEVELS - 1; i >= 0; i--) {
        while(cur != NULL) {
            struct skiplist_node *next = cur->levels[i].next;
            if (can_move_forward(next, score, key)) {
                rank += cur->levels[i].span;
                cur = next;
            }else {
                break;
            }
        }
    }
    if (cur == s->head) {
        cur=s->head->levels[0].next;
        rank++;
    }
    for (; cur != NULL && compare_str(key, cur->key) != 0; cur = cur->levels[0].next) {
        rank++;
    }
    if (cur == NULL) return 0;
    return rank;
}

#ifdef DEBUG
void print_skiplist(struct skiplist *s)
{
    for (int i = MAX_LEVELS - 1; i >= 0; i--) {
        struct skiplist_node *node = s->head;
        printf("%d: ", i); 
        while(node != NULL) {
            printf("(%3.0f,%3d) ", node->score,node->levels[i].span);
            for (int j = 1; j < node->levels[i].span; j++) printf("          ");
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
    s->rank = skiplist_rank;
    s->remove = skiplist_remove;
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