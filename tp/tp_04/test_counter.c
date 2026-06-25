/*
 * test_counter.c — Compteur partagé protégé par mutex
 * Sortie attendue : "SUCCÈS : compteur == 400000"
 */
#include "mthread_mutex.h"
#include "../tp_03/mthread.h"
#include <stdio.h>
#include <stdlib.h>

#define N_THREADS  4
#define N_ITER     100000

long compteur = 0;
mthread_mutex_t mutex = MTHREAD_MUTEX_INIT;

void *incrementer(void *arg) {
    (void)arg;
    for (int i = 0; i < N_ITER; i++) {
        mthread_mutex_lock(&mutex);
        compteur++;
        mthread_mutex_unlock(&mutex);
        if (i % 10000 == 0)
            mthread_yield();   /* coopérer régulièrement */
    }
    return NULL;
}

int main(void) {
    mthread_t t[N_THREADS];

    for (int i = 0; i < N_THREADS; i++)
        mthread_create(&t[i], incrementer, NULL);

    mthread_run();

    for (int i = 0; i < N_THREADS; i++)
        mthread_join(t[i], NULL);

    long attendu = (long)N_THREADS * N_ITER;
    if (compteur == attendu)
        printf("SUCCÈS : compteur == %ld\n", compteur);
    else
        printf("ÉCHEC  : compteur == %ld (attendu %ld)\n", compteur, attendu);

    return compteur == attendu ? 0 : 1;
}
