#include "mthread.h"
#include <stdio.h>
#include <assert.h>

#define N_THREADS 100
#define N_YIELDS  100

static volatile int done_count = 0;

void *stress_worker(void *arg) {
    (void)arg;
    for (int i = 0; i < N_YIELDS; i++)
        mthread_yield();
    __atomic_fetch_add(&done_count, 1, __ATOMIC_SEQ_CST);
    return NULL;
}

int main(void) {
    mthread_t threads[N_THREADS];
    for (int i = 0; i < N_THREADS; i++)
        mthread_create(&threads[i], stress_worker, NULL);
    for (int i = 0; i < N_THREADS; i++)
        mthread_join(threads[i], NULL);
    printf("done_count = %d (attendu : %d)\n", done_count, N_THREADS);
    assert(done_count == N_THREADS);
    printf("[PASS]\n");
    return 0;
}
