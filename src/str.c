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
        int res=0;
        for(int i = 0; i < a0->length; i++) {
            if (a0->buf[i] < b0->buf[i]) res-=1;
            else if(a0->buf[i] > b0->buf[i]) res+=1;
        }
        return res;
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