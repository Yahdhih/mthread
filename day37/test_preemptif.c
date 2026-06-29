#include "mthread.h"
#include "preempt.c"
#include <stdio.h>
#include <unistd.h>

volatile int stop = 0;
volatile long counts[2] = {0, 0};

void *cpu_bound(void *arg) {
    int id = *(int *)arg;
    while (!stop) {
        counts[id]++;
        /* Pas de yield explicite — dépend de la préemption */
    }
    printf("Thread %d : %ld itérations\n", id, counts[id]);
    return NULL;
}

int main(void) {
    mthread_preempt_enable();

    mthread_t t1, t2;
    int ids[2] = {0, 1};
    mthread_create(&t1, cpu_bound, &ids[0]);
    mthread_create(&t2, cpu_bound, &ids[1]);

    /* Laisser tourner 1 seconde */
    sleep(1);
    stop = 1;

    mthread_join(t1, NULL);
    mthread_join(t2, NULL);

    mthread_preempt_disable();

    printf("Thread 0 : %ld itérations\n", counts[0]);
    printf("Thread 1 : %ld itérations\n", counts[1]);

    if (counts[0] > 0 && counts[1] > 0)
        printf("[PASS] Les deux threads ont progressé grâce à la préemption\n");
    else
        printf("[FAIL] Un thread n'a pas progressé — la préemption ne fonctionne pas\n");

    return 0;
}
