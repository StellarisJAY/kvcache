#ifndef STR_H
#define STR_H

struct str 
{
    char *buf;
    unsigned int length;
};

struct str *from_char_array(char *buf, unsigned int len);
struct str *with_char_array(char *buf, unsigned int len);
struct str *copy_str(struct str *src);
void str_to_upper(struct str *src);
int compare_str(void *a, void *b);
unsigned long long str_hash_func(void *s);
#endif