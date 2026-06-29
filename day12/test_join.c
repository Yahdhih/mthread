#include "mthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void *compute(void *arg) {
    int n = *(int *)arg;
    printf("Thread calcule %d * 2 = %d\n", n, n*2);
    int *result = malloc(sizeof(int));
    *result = n * 2;
    return result;
}

void *no_return(void *arg) {
    (void)arg;
    printf("Thread sans retour\n");
    return NULL;
}

int main(void) {
    /* Test 1 : join avec valeur de retour */
    mthread_t t;
    int n = 21;
    mthread_create(&t, compute, &n);
    void *rv;
    mthread_join(t, &rv);
    printf("Résultat : %d\n", *(int *)rv);
    assert(*(int *)rv == 42);
    free(rv);

    /* Test 2 : join sans récupérer la valeur */
    mthread_t t2;
    mthread_create(&t2, no_return, NULL);
    mthread_join(t2, NULL);

    printf("[PASS]\n");
    return 0;
}
