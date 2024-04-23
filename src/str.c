#include "str.h"
#include <stdlib.h>
#include <string.h>
#include "highwayhash.h"

struct str *from_char_array(char *arr, unsigned int len)
{
    struct str *s = malloc(sizeof(struct str));
    s->buf = malloc(len);
    memcpy(s->buf, arr, len);
    s->length = len;    
    return s;
}

struct str *with_char_array(char *arr, unsigned int len)
{
    struct str *s = malloc(sizeof(struct str));
    s->buf = arr;
    s->length = len;
    return s;
}

int compare_str(void *a, void *b)
{
    struct str *a0 = a;
    struct str *b0 = b;
    if (a0->length == b0->length) {
        return strcmp(a0->buf, b0->buf);
    }else {
        return a0->length - b0->length;
    }
}

unsigned long long str_hash_func(void *s)
{
    struct str *s0 = s;
    const uint8_t *data = s0->buf;
    const uint64_t key[4] = {1, 2, 3, 4};
    return HighwayHash64(data, s0->length, key);
}