#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#define EVENT_NONE 0
#define EVENT_READ 1
#define EVENT_WRITE 2
#define EVENT_HUP 4
#define EVENT_ERR 8

#include <stdlib.h>

struct event
{
    int fd;
    int mask;
};

struct eventloop 
{
    unsigned int size;
    void *data;
    struct event *ready_events;
};

extern int eventloop_init(struct eventloop *el);
extern int eventloop_add(struct eventloop *el, int fd, int mask);
extern int eventloop_delete(struct eventloop *el, int fd);
extern int eventloop_wait(struct eventloop *el);
extern void eventloop_free(struct eventloop *el);

static struct eventloop *create_eventloop(int max_conn)
{
    struct eventloop *el = malloc(sizeof(struct eventloop));
    el->size = max_conn;
    el->ready_events = malloc(sizeof(struct event) * el->size);
    eventloop_init(el);
    return el;
}
#endif