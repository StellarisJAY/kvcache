#include "thread_pool.h"
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#ifdef DEBUG
#include <string.h>
#include <stdio.h>
#endif

static struct task *task_queue_pop(struct threadpool *pool)
{
    if (pool->pending == 1) {
        struct task *res = pool->task_queue_head;
        pool->task_queue_head = pool->task_queue_tail = NULL;
        res->prev = res->next = NULL;
        pool->pending--;
        return res;
    }else {
        struct task *res = pool->task_queue_head;
        pool->task_queue_head = res->next;
        pool->task_queue_head->prev = NULL;
        res->next = res->prev = NULL;
        pool->pending--;
        return res;
    }
}

static void task_queue_push(struct threadpool *pool, struct task *task)
{
    if (pool->pending == 0) {
        pool->task_queue_head = pool->task_queue_tail = task;
        pool->pending = 1;
    }else {
        task->prev = pool->task_queue_tail;
        pool->task_queue_tail->next = task;
        pool->task_queue_tail = task;
        pool->pending++;
    }
}

static void *worker_func(void *args)
{
    #ifdef DEBUG
    printf("[DEBUG] worker started...\n");
    #endif
    struct threadpool *pool = args;
    while(1) {
        #ifdef DEBUG
        printf("[DEBUG] worker waiting tasks\n");
        #endif
        pthread_mutex_lock(&pool->mutex);
        if (pool->pending == 0 && pool->state == SHUTDOWN) {
            pthread_mutex_unlock(&pool->mutex);
            return NULL;
        }
        if (pool->pending == 0) pthread_cond_wait(&pool->cond, &pool->mutex);
        if (pool->pending == 0 && pool->state == SHUTDOWN) {
            pthread_mutex_unlock(&pool->mutex);
            return NULL;
        }
        struct task *task = task_queue_pop(pool);
        pthread_mutex_unlock(&pool->mutex);
        #ifdef DEBUG
        printf("[DEBUG] worker got task, func:%p, args:%p\n", task->func, task->args);
        #endif
        task->func(task->args);
        free(task);
    }
}

void threadpool_enqueue(struct threadpool *pool, task_func func, void *args)
{
    struct task *task = malloc(sizeof(struct task));
    task->func = func;
    task->args = args;
    task->next = task->prev = NULL;
    pthread_mutex_lock(&pool->mutex);
    if (pool->state == SHUTDOWN) {
        pthread_mutex_unlock(&pool->mutex);
        free(task);
        return;
    }
    task_queue_push(pool, task);
    if (pool->pending == 1) pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->mutex);
}

void threadpool_start(struct threadpool *pool)
{
    pthread_mutex_lock(&pool->mutex);
    if (pool->state != INIT) {
        pthread_mutex_unlock(&pool->mutex);
        return;
    }
    pool->state = RUNNING;
    pthread_mutex_unlock(&pool->mutex);
    for (int i = 0; i < pool->size; i++) {
        pthread_t t;
        pthread_create(&t, NULL, worker_func, pool);
        pool->workers[i] = t;
    }
}

void threadpool_shutdown(struct threadpool *pool)
{
    pthread_mutex_lock(&pool->mutex);
    if (pool->state == SHUTDOWN) {
        pthread_mutex_unlock(&pool->mutex);
        return;
    }
    pool->state = SHUTDOWN;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->mutex);
    for (int i = 0; i < pool->size; i++) {
        pthread_join(pool->workers[i], NULL);
    }
    free(pool->workers);
    free(pool);
}

struct threadpool *create_threadpool(int workers)
{
    struct threadpool *pool = malloc(sizeof(struct threadpool));
    pthread_cond_init(&pool->cond, NULL);
    pthread_mutex_init(&pool->mutex, NULL);
    pool->workers = malloc(sizeof(pthread_t));
    pool->size = workers;
    pool->pending = 0;
    pool->task_queue_head = pool->task_queue_tail = NULL;
    pool->enqueue = threadpool_enqueue;
    pool->start = threadpool_start;
    pool->shutdown = threadpool_shutdown;
    return pool;
}