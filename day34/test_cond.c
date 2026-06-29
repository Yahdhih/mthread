#include "mthread.h"
#include "mthread_mutex.h"
#include "mthread_cond.h"
#include <stdio.h>
#include <assert.h>

/* Test 1 : signal simple */
static int ready=0;
static mthread_mutex_t mu=MTHREAD_MUTEX_INITIALIZER;
static mthread_cond_t cv=MTHREAD_COND_INITIALIZER;

void *waiter(void *a) { (void)a;
    mthread_mutex_lock(&mu);
    while (!ready) mthread_cond_wait(&cv,&mu);
    assert(ready==1);
    mthread_mutex_unlock(&mu);
    return NULL;
}
void *setter(void *a) { (void)a;
    mthread_mutex_lock(&mu);
    ready=1;
    mthread_cond_signal(&cv);
    mthread_mutex_unlock(&mu);
    return NULL;
}

/* Test 2 : broadcast — N threads attendent */
#define N_WAITERS 5
static int go=0;
static int woke=0;
static mthread_mutex_t bmu=MTHREAD_MUTEX_INITIALIZER;
static mthread_cond_t bcv=MTHREAD_COND_INITIALIZER;

void *bcast_waiter(void *a) { (void)a;
    mthread_mutex_lock(&bmu);
    while (!go) mthread_cond_wait(&bcv,&bmu);
    woke++;
    mthread_mutex_unlock(&bmu);
    return NULL;
}

int main(void) {
    printf("=== Test 1 : signal ===\n");
    mthread_t tw,ts;
    mthread_create(&tw,waiter,NULL);
    mthread_create(&ts,setter,NULL);
    mthread_join(tw,NULL); mthread_join(ts,NULL);
    printf("[PASS]\n");

    printf("=== Test 2 : broadcast ===\n");
    mthread_t bw[N_WAITERS], bs;
    for (int i=0;i<N_WAITERS;i++) mthread_create(&bw[i],bcast_waiter,NULL);
    mthread_mutex_lock(&bmu);
    go=1;
    mthread_cond_broadcast(&bcv);
    mthread_mutex_unlock(&bmu);
    for (int i=0;i<N_WAITERS;i++) mthread_join(bw[i],NULL);
    assert(woke==N_WAITERS);
    printf("[PASS] %d threads réveillés\n", woke);

    printf("\n[ALL PASS]\n");
    return 0;
}
