#include "mthread.h"
#include "mthread_mutex.h"
#include "mthread_cond.h"
#include <stdio.h>
#include <assert.h>

#define BUF_SIZE 3
#define N_ITEMS  9

static int buffer[BUF_SIZE];
static int head=0, tail=0, count=0;
static int total_consumed = 0;

static mthread_mutex_t mu       = MTHREAD_MUTEX_INITIALIZER;
static mthread_cond_t  not_full = MTHREAD_COND_INITIALIZER;
static mthread_cond_t  not_empty= MTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    (void)arg;
    for (int i = 0; i < N_ITEMS; i++) {
        mthread_mutex_lock(&mu);
        while (count == BUF_SIZE)
            mthread_cond_wait(&not_full, &mu);
        buffer[tail % BUF_SIZE] = i;
        tail++;
        count++;
        printf("[P] produit %d (count=%d)\n", i, count);
        mthread_cond_signal(&not_empty);
        mthread_mutex_unlock(&mu);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < N_ITEMS / 2; i++) {  /* chaque C prend la moitié */
        mthread_mutex_lock(&mu);
        while (count == 0)
            mthread_cond_wait(&not_empty, &mu);
        int val = buffer[head % BUF_SIZE];
        head++;
        count--;
        total_consumed++;
        printf("[C%d] consommé %d (count=%d)\n", id, val, count);
        mthread_cond_signal(&not_full);
        mthread_mutex_unlock(&mu);
    }
    return NULL;
}

int main(void) {
    mthread_t p, c1, c2;
    int ids[2] = {1, 2};

    mthread_create(&p,  producer,  NULL);
    mthread_create(&c1, consumer,  &ids[0]);
    mthread_create(&c2, consumer,  &ids[1]);

    mthread_join(p,  NULL);
    mthread_join(c1, NULL);
    mthread_join(c2, NULL);

    printf("total produit=%d total consommé=%d\n", N_ITEMS, total_consumed);
    assert(total_consumed == N_ITEMS - 1); /* 2 consommateurs × 4 = 8, 1 reste dans buffer */
    printf("[PASS]\n");
    return 0;
}
