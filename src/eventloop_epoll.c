#include "eventloop.h"
#include <sys/errno.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct epoll_state
{
    int epfd;
    struct epoll_event *events;
};

int eventloop_init(struct eventloop *el)
{
    struct epoll_state *state = malloc(sizeof(struct epoll_state)); 
    int fd = epoll_create(el->size);
    if (fd < 0) {
        free(state);
        return -1;
    } 
    state->epfd = fd;
    struct epoll_event *events = malloc(sizeof(struct epoll_event) * el->size);
    state->events = events;
    el->data = state;
    return 0;
}

int eventloop_add(struct eventloop *el, int fd, int mask)
{
    struct epoll_state *state = el->data;
    struct epoll_event event;
    event.data.fd = fd;
    if (mask & EVENT_READ != 0) event.events |= EPOLLIN;
    if (mask & EVENT_WRITE != 0) event.events |= EPOLLOUT;
    if (mask & EVENT_HUP) event.events |= EPOLLHUP | EPOLLRDHUP;
    if (mask & EVENT_ERR) event.events |= EPOLLERR;
    if (epoll_ctl(state->epfd, EPOLL_CTL_ADD, fd, &event) < 0) {
        printf("epoll_ctl_add error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int eventloop_delete(struct eventloop *el, int fd)
{
    struct epoll_state *state = el->data;
    struct epoll_event event;
    if (epoll_ctl(state->epfd, EPOLL_CTL_DEL, fd, &event) < 0) {
        printf("epoll_ctl_del error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int eventloop_wait(struct eventloop *el)
{
    struct epoll_state *state = el->data;
    int n = epoll_wait(state->epfd, state->events, el->size, -1);
    if (n < 0) {
        printf("epoll_wait error: %s\n", strerror(errno));
        return -1;
    }
    for (int i = 0; i < n; i++) {
        struct epoll_event ev = state->events[i];
        int mask = EVENT_NONE;
        if (ev.events & EPOLLIN) mask |=  EVENT_READ;
        if (ev.events & EPOLLOUT) mask |= EVENT_WRITE;
        if (ev.events & EPOLLERR) mask |= EVENT_ERR;
        if (ev.events & EPOLLHUP || ev.events & EPOLLRDHUP) mask |= EVENT_HUP;
        el->ready_events[i].fd = ev.data.fd;
        el->ready_events[i].mask = mask;
    }
    return n;
}

void eventloop_free(struct eventloop *el)
{
    struct epoll_state *state = el->data;
    free(state->events);
    free(state);
}