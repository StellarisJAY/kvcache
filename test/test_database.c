#include "test.h"
#include "db.h"
#include <stdlib.h>
#include <stdio.h>
#include "str.h"
void test_database_str()
{
    struct database *db = create_database();
    
    struct str *key1 = from_char_array("key1", 4);
    struct str *key2 = from_char_array("key2", 4);
    struct str *key3 = from_char_array("key3", 4);

    struct str *val1 = from_char_array("val1", 4);
    struct str *val2 = from_char_array("val2", 4);
    struct str *val3 = from_char_array("val3", 4);

    if (db->set_str(db, 0, key1, val1) != 1) {
        printf("test set str key1 failed\n");
        return;
    }
    db->set_str(db, 0, key1, val2);
    struct str *ret1 = db->get_str(db, 0, key1);
    if (ret1 == NULL || compare_str(ret1, val2) != 0) {
        printf("get key1 failed, expect: %p, got: %p\n", val2, ret1);
        return;
    }

    printf("test database str passed!\n");
}