#include "mthread.h"
#include "mthread_mutex.h"
#include <stdio.h>
#include <assert.h>

#define N_THREADS 10
#define N_ITER    10000

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

void *test_trylock(void *arg) {
    (void)arg;
    /* trylock ne doit pas bloquer */
    int r1 = mthread_mutex_trylock(&mu);
    if (r1 == 0) {
        printf("trylock : acquis\n");
        int r2 = mthread_mutex_trylock(&mu);
        assert(r2 == -1);  /* déjà pris */
        printf("trylock (déjà pris) : -1 (correct)\n");
        mthread_mutex_unlock(&mu);
    } else {
        printf("trylock : occupé (r1=%d)\n", r1);
    }
    return NULL;
}

int main(void) {
    printf("=== Test 1 : compteur concurrent ===\n");
    mthread_t threads[N_THREADS];
    for (int i = 0; i < N_THREADS; i++)
        mthread_create(&threads[i], increment, NULL);
    for (int i = 0; i < N_THREADS; i++)
        mthread_join(threads[i], NULL);
    printf("counter = %ld (attendu : %d)\n", counter, N_THREADS * N_ITER);
    assert(counter == N_THREADS * N_ITER);
    printf("[PASS] counter correct\n");

    printf("=== Test 2 : trylock ===\n");
    mthread_t t;
    mthread_create(&t, test_trylock, NULL);
    mthread_join(t, NULL);
    printf("[PASS] trylock\n");

    printf("\n[ALL PASS]\n");
    return 0;
}
