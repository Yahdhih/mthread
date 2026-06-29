#include "mthread.h"
#include <stdio.h>

void *worker(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 3; i++) {
        printf("Thread %d — itération %d\n", id, i);
        mthread_yield();
    }
    printf("Thread %d — terminé\n", id);
    return NULL;
}

int main(void) {
    mthread_t t1, t2, t3;
    int ids[3] = {1, 2, 3};
    mthread_create(&t1, worker, &ids[0]);
    mthread_create(&t2, worker, &ids[1]);
    mthread_create(&t3, worker, &ids[2]);
    mthread_yield();
    printf("Main terminé\n");
    return 0;
}
