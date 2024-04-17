#ifndef STR_H
#define STR_H

struct str 
{
    char *buf;
    unsigned int length;
};

struct str *from_char_array(char *buf, unsigned int len);
int compare_str(void *a, void *b);
unsigned long long str_hash_func(void *s);
#endif