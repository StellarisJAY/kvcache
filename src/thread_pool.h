#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>

typedef void *(*task_func)(void *args);

struct task
{
    task_func func;
    void *args;
    struct task *next;
    struct task *prev;
};

enum threadpool_state
{
    INIT,
    RUNNING,
    SHUTDOWN,
};

struct threadpool 
{
    pthread_t *workers;
    int size;
    struct task *task_queue_head;
    struct task *task_queue_tail;
    int pending;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    enum threadpool_state state;

    void (*enqueue)(struct threadpool*, task_func, void *);
    void (*start)(struct threadpool*);
    void (*shutdown)(struct threadpool*);
};

struct threadpool *create_threadpool(int workers);
#endif