#include "mthread.h"
#include "mthread_sem.h"
#include <stdio.h>
#include <assert.h>

/* Test 1 : producteur / consommateur */
#define BUF_SIZE 5
#define N_ITEMS  20
static int buf[BUF_SIZE], in_i=0, out_i=0;
static mthread_sem_t sem_e, sem_f;

void *prod(void *a) { (void)a;
    for (int i=0;i<N_ITEMS;i++) {
        mthread_sem_wait(&sem_e);
        buf[in_i%BUF_SIZE]=i; in_i++;
        mthread_sem_post(&sem_f);
    }
    return NULL;
}
void *cons(void *a) { (void)a;
    for (int i=0;i<N_ITEMS;i++) {
        mthread_sem_wait(&sem_f);
        out_i++;
        mthread_sem_post(&sem_e);
    }
    return NULL;
}

/* Test 2 : trywait */
static mthread_sem_t ts;
void *try_worker(void *a) { (void)a;
    assert(mthread_sem_trywait(&ts)==0);   /* valeur=1 -> ok */
    assert(mthread_sem_trywait(&ts)==-1);  /* valeur=0 -> busy */
    mthread_sem_post(&ts);
    return NULL;
}

int main(void) {
    printf("=== Test 1 : prod/conso ===\n");
    mthread_sem_init(&sem_e,BUF_SIZE); mthread_sem_init(&sem_f,0);
    mthread_t p,c;
    mthread_create(&p,prod,NULL); mthread_create(&c,cons,NULL);
    mthread_join(p,NULL); mthread_join(c,NULL);
    assert(in_i==N_ITEMS && out_i==N_ITEMS);
    printf("[PASS]\n");

    printf("=== Test 2 : trywait ===\n");
    mthread_sem_init(&ts,1);
    mthread_t t; mthread_create(&t,try_worker,NULL); mthread_join(t,NULL);
    printf("[PASS]\n");

    printf("\n[ALL PASS]\n");
    return 0;
}
