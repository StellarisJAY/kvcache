#include "hashmap.h"
#include "str.h"
#include <stdlib.h>
#include <stdio.h>

void test_hashmap()
{
    struct hashmap *hmap = create_hashmap(compare_str, str_hash_func);
    struct str *key1 = from_char_array("key1", 4);
    struct str *key2 = from_char_array("key2", 4);
    struct str *key3 = from_char_array("key3", 4);
    int val1 = 10;
    int val2 = 20;
    unsigned int key_len = sizeof(struct str);
    hmap->op.hash_put(hmap, key1, key_len, &val1);
    hmap->op.hash_put(hmap, key2, key_len, &val1);
    hmap->op.hash_put(hmap, key1, key_len, &val2);
    int *ret1 = hmap->op.hash_get(hmap, key1, key_len);
    if (ret1 == NULL) {
        printf("test key1 failed\n");
        return;
    }
    if (*ret1 != val2) {
        printf("get key1 expect=%d,got=%d\n", val2, *ret1);
        return;
    }

    int *ret2 = hmap->op.hash_get(hmap, key2, key_len);
    if (ret2 == NULL) {
        printf("test key2 failed\n");
        return;
    }
    if (*ret2 != val1) {
        printf("get key2 expect=%d,got=%d\n", val1, *ret2);
        return;
    }

    int *ret3 = hmap->op.hash_get(hmap, key3, key_len);
    if (ret3 != NULL) {
        printf("test key3 failed, expect=%p,got=%p\n", NULL, ret3);
        return;
    }
    printf("hashmap test passed\n");
}