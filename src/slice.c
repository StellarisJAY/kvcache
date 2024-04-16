#include "slice.h"
#include <stdlib.h>

void resize(struct slice *self, unsigned int newCap)
{
    self->data = realloc(self->data, newCap * sizeof(void *));
    self->cap = newCap;
}

void slice_add(struct slice *self, void *val)
{
    if (self->size == self->cap) {
        resize(self, self->cap * 2);
    }
    self->data[self->size] = val;
    self->size++;
}

void slice_set(struct slice *self, unsigned int idx, void *val)
{
    if (idx < self->size) {
        self->data[idx] = val;
    }
}

void *slice_get(struct slice *self, unsigned int idx)
{
    if (idx >= self->size) {
        return NULL;
    }
    return self->data[idx];
}

void slice_free(struct slice *self)
{
    for (int i = 0; i < self->size; i++) {
        free(self->data[i]);
    }
    free(self);
}

struct slice *create_slice(unsigned int size, unsigned int cap)
{
    if (cap == 0 || size > cap) {
        return NULL;
    }
    struct slice *s = malloc(sizeof(struct slice));
    s->data = malloc(sizeof(void *) * cap);
    s->cap = cap;
    s->size = size;
    struct slice_op op = {
        .add = slice_add,
        .set = slice_set,
        .get = slice_get, 
        .free = slice_free,
    };
    s->op = op;
    return s;
}