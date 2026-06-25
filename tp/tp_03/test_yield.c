/*
 * test_yield.c — 3 threads qui s'alternent via yield
 * Sortie attendue (ordre stable avec scheduler FIFO) :
 *   Thread 0 : tour 1
 *   Thread 1 : tour 1
 *   Thread 2 : tour 1
 *   Thread 0 : tour 2
 *   Thread 1 : tour 2
 *   Thread 2 : tour 2
 *   Thread 0 : tour 3
 *   ...
 */
#include "mthread.h"
#include <stdio.h>
#include <stdlib.h>

void *worker(void *arg) {
    int id = *(int *)arg;
    for (int i = 1; i <= 3; i++) {
        printf("Thread %d : tour %d\n", id, i);
        mthread_yield();
    }
    return NULL;
}

int main(void) {
    mthread_t t[3];
    int ids[3] = {0, 1, 2};

    for (int i = 0; i < 3; i++)
        mthread_create(&t[i], worker, &ids[i]);

    mthread_run();   /* démarre le scheduler */

    /* join après que tous les threads aient fini */
    for (int i = 0; i < 3; i++)
        mthread_join(t[i], NULL);

    printf("Tous terminés.\n");
    return 0;
}
