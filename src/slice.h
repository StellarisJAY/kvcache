#ifndef SLICE_H
#define SLICE_H
struct slice {
    void **data;
    unsigned int size;
    unsigned int cap;

    struct slice_op {
        void (*add)(struct slice *self, void *val);
        void (*set)(struct slice *self, unsigned int idx, void *val);
        void *(*get)(struct slice *self, unsigned int idx);
        void (*free)(struct slice *self);
    } op;
};

struct slice *create_slice(unsigned int size, unsigned int cap);
#endif