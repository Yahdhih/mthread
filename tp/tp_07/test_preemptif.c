/*
 * test_preemptif.c — Vérifier que la préemption fonctionne
 *
 * T1 calcule sans jamais appeler yield.
 * Sans préemption : T2 ne s'exécuterait jamais.
 * Avec préemption (SIGALRM) : T2 devrait afficher ses messages.
 *
 * Sortie attendue (messages entrelacés) :
 *   T1 calcule...
 *   T2 : je suis vivant ! (1)
 *   T1 calcule...
 *   T2 : je suis vivant ! (2)
 *   ...
 */
#include "../tp_03/mthread.h"
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

#define QUANTUM_MS  10

/* TODO : implémenter enable_preemption ici ou dans mthread.c */
extern void scheduler_enable_preemption(int quantum_ms);

volatile int stop = 0;

void *calcul_gourmand(void *arg) {
    (void)arg;
    long i = 0;
    while (!stop) {
        i++;
        if (i % 10000000 == 0)
            printf("T1 calcule... (i=%ld)\n", i);
    }
    return NULL;
}

void *afficheur(void *arg) {
    (void)arg;
    for (int n = 1; n <= 5; n++) {
        printf("T2 : je suis vivant ! (%d)\n", n);
        /* Pas de yield intentionnel — la préemption doit nous donner le CPU */
        for (volatile long j = 0; j < 5000000; j++);
    }
    stop = 1;
    return NULL;
}

int main(void) {
    scheduler_enable_preemption(QUANTUM_MS);

    mthread_t t1, t2;
    mthread_create(&t1, calcul_gourmand, NULL);
    mthread_create(&t2, afficheur, NULL);

    mthread_run();

    mthread_join(t1, NULL);
    mthread_join(t2, NULL);

    printf("Préemption validée.\n");
    return 0;
}
