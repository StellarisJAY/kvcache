#include "test.h"
#include "thread_pool.h"
#include <stdio.h>
#include <unistd.h>

void *task1(void *args)
{
    int sum = 0;
    for (int i = 0; i < 10000; i++) {
        sum += i;
    }
    printf("sum=%d\n", sum);
}

void test_threadpool()
{
    struct threadpool *pool = create_threadpool(2);
    pool->start(pool);
    pool->enqueue(pool, task1, NULL);
    pool->enqueue(pool, task1, NULL);
    sleep(3);
    pool->shutdown(pool);
    printf("threadpool test finished!\n");
}