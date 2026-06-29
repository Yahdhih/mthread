/* tests/test_threads.c — Test complet de create/yield/exit/join */
#include "mthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Test 1 : 3 threads qui yield en alternance */
static int order[9];
static int order_idx = 0;

void *round_robin_worker(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 3; i++) {
        order[order_idx++] = id;
        mthread_yield();
    }
    return NULL;
}

/* Test 2 : valeur de retour via join */
void *compute_double(void *arg) {
    int n = *(int *)arg;
    int *r = malloc(sizeof(int));
    *r = n * 2;
    return r;
}

/* Test 3 : chaîne de joins */
mthread_t chain[4];
void *chain_worker(void *arg) {
    int id = *(int *)arg;
    if (id > 0) mthread_join(chain[id-1], NULL);
    printf("chain thread %d\n", id);
    return NULL;
}

int main(void) {
    printf("=== Test 1 : Round-Robin ===\n");
    mthread_t t[3];
    int ids[3] = {0, 1, 2};
    for (int i = 0; i < 3; i++)
        mthread_create(&t[i], round_robin_worker, &ids[i]);
    for (int i = 0; i < 3; i++)
        mthread_join(t[i], NULL);
    /* Vérifier l'alternance : 0,1,2,0,1,2,0,1,2 */
    for (int i = 0; i < 9; i++)
        assert(order[i] == i % 3);
    printf("[PASS] Round-Robin correct\n");

    printf("=== Test 2 : valeur de retour ===\n");
    mthread_t t2;
    int n = 21;
    mthread_create(&t2, compute_double, &n);
    void *rv;
    mthread_join(t2, &rv);
    assert(*(int *)rv == 42);
    free(rv);
    printf("[PASS] Valeur de retour = 42\n");

    printf("=== Test 3 : chaîne de joins ===\n");
    int cids[4] = {0, 1, 2, 3};
    for (int i = 0; i < 4; i++)
        mthread_create(&chain[i], chain_worker, &cids[i]);
    mthread_join(chain[3], NULL);
    printf("[PASS] Chaîne de joins\n");

    printf("\n[ALL PASS]\n");
    return 0;
}
