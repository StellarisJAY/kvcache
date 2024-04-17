#include "test.h"
#include "skiplist.h"
#include "str.h"
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
    #ifdef DEBUG
    s->print_skiplist(s);
    #endif
}