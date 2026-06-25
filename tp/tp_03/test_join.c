/*
 * test_join.c — Thread parent attend la valeur de retour de l'enfant
 * Sortie attendue :
 *   Enfant calcule 42...
 *   Parent reçoit : 42
 */
#include "mthread.h"
#include <stdio.h>
#include <stdlib.h>

void *calculer(void *arg) {
    (void)arg;
    printf("Enfant calcule 42...\n");
    mthread_yield();   /* simule du travail */
    int *res = malloc(sizeof(int));
    *res = 42;
    return res;
}

int main(void) {
    mthread_t t;
    mthread_create(&t, calculer, NULL);
    mthread_run();

    void *retval;
    mthread_join(t, &retval);
    printf("Parent reçoit : %d\n", *(int *)retval);
    free(retval);
    return 0;
}
