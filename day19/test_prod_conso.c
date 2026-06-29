#include "mthread.h"
#include "mthread_sem.h"
#include <stdio.h>
#include <assert.h>

#define BUF_SIZE 5
#define N_ITEMS  15

static int buffer[BUF_SIZE];
static int in_idx = 0, out_idx = 0;

static mthread_sem_t sem_empty, sem_full;

void *producer(void *arg) {
    (void)arg;
    for (int i = 0; i < N_ITEMS; i++) {
        mthread_sem_wait(&sem_empty);
        buffer[in_idx % BUF_SIZE] = i;
        in_idx++;
        printf("[P] produit %d (in=%d)\n", i, in_idx);
        mthread_sem_post(&sem_full);
    }
    return NULL;
}

void *consumer(void *arg) {
    (void)arg;
    for (int i = 0; i < N_ITEMS; i++) {
        mthread_sem_wait(&sem_full);
        int val = buffer[out_idx % BUF_SIZE];
        out_idx++;
        printf("[C] consommé %d (out=%d)\n", val, out_idx);
        mthread_sem_post(&sem_empty);
    }
    return NULL;
}

int main(void) {
    mthread_sem_init(&sem_empty, BUF_SIZE);
    mthread_sem_init(&sem_full,  0);

    mthread_t p, c;
    mthread_create(&p, producer, NULL);
    mthread_create(&c, consumer, NULL);
    mthread_join(p, NULL);
    mthread_join(c, NULL);

    printf("in_idx=%d out_idx=%d\n", in_idx, out_idx);
    assert(in_idx == N_ITEMS && out_idx == N_ITEMS);
    printf("[PASS]\n");
    return 0;
}
