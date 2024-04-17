#include "test.h"
#include "skiplist.h"
#include "str.h"
#include "zset.h"
#include <stdio.h>

void test_skiplist()
{
    struct skiplist *s = create_skiplist();
    struct str *k1 = from_char_array("k1", 2);
    struct str *k2 = from_char_array("k2", 2);
    struct str *k3 = from_char_array("k3", 2);
    s->insert(s, k1, 2.0);
    s->insert(s, k2, 1.0);
    s->insert(s, k3, -1.0);


    printf("%s rank=%d\n", k1->buf, s->rank(s, k1, 2.0));
    printf("%s rank=%d\n", k2->buf, s->rank(s, k2, 1.0));
    printf("%s rank=%d\n", k3->buf, s->rank(s, k3, -1.0));

    #ifdef DEBUG
    s->print_skiplist(s);
    #endif
}


void test_sorted_set()
{
    struct sorted_set *zset = create_zset();
    struct str *k1 = from_char_array("k1", 2);
    struct str *k2 = from_char_array("k2", 2);
    struct str *k3 = from_char_array("k3", 2);
    double score1 = 2.0;
    double score2 = 1.0;
    double score3 = -1.0;

    zset->insert(zset, k1, score1);
    zset->insert(zset, k3, score3);
    zset->insert(zset, k2, score2);

    printf("%s rank=%d\n", k1->buf, zset->rank(zset, k1));
    printf("%s rank=%d\n", k2->buf, zset->rank(zset, k2));
    printf("%s rank=%d\n", k3->buf, zset->rank(zset, k3));
    #ifdef DEBUG
    zset->list->print_skiplist(zset->list);
    #endif
}