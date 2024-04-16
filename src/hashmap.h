#ifndef HASHMAP_H
#define HASHMAP_H

#define BUCKET_SIZE 8

struct bucket {
    long tophash[BUCKET_SIZE];
    void *keys[BUCKET_SIZE];
    void *vals[BUCKET_SIZE];
    struct bucket *overflow;
};

struct hashmap {
    struct bucket *bmap;
    unsigned int size;
    unsigned int bmap_size;
    unsigned int B;

    struct hashmap_op {
        void (*hash_put)(struct hashmap *hmap, void *key, unsigned int keyLen, void *val);
        void *(*hash_get)(struct hashmap *hmap, void *key, unsigned int keyLen);
        void *(*hash_del)(struct hashmap *hmap, void *key, unsigned int keyLen);
        unsigned long long (*hash_func)(void *key, unsigned int keyLen);
        void (*free)(struct hashmap *hamp);
    } op;

    int (*compare)(void *key1, void *key2);
};

struct hashmap *create_hashmap(int (*compare)(void*, void*), unsigned long long (*hash_func)(void*, unsigned int));
#endif