#include "mthread.h"
#include "mthread_mutex.h"
#include <stdio.h>
#include <assert.h>

#define N_THREADS 5
#define N_ITER    1000

static mthread_mutex_t mu = MTHREAD_MUTEX_INITIALIZER;
static long counter = 0;

void *increment(void *arg) {
    (void)arg;
    for (int i = 0; i < N_ITER; i++) {
        mthread_mutex_lock(&mu);
        counter++;
        mthread_mutex_unlock(&mu);
    }
    return NULL;
}

int main(void) {
    mthread_t threads[N_THREADS];
    for (int i = 0; i < N_THREADS; i++)
        mthread_create(&threads[i], increment, NULL);
    for (int i = 0; i < N_THREADS; i++)
        mthread_join(threads[i], NULL);
    printf("counter = %ld (attendu : %d)\n", counter, N_THREADS * N_ITER);
    if (counter == N_THREADS * N_ITER)
        printf("[PASS]\n");
    else
        printf("[FAIL] — mutex pas encore correct ?\n");
    return 0;
}
