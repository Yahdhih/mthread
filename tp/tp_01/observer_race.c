/*
 * TP 01 - observer_race.c
 * Race condition volontaire sur un compteur partagé.
 * Lance avec : valgrind --tool=helgrind ./observer_race
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N_THREADS   4
#define N_ITER      100000

long compteur = 0;  /* variable partagée NON protégée */

void *incrementer(void *arg) {
    for (int i = 0; i < N_ITER; i++)
        compteur++;   /* race condition ici */
    return NULL;
}

int main(void) {
    pthread_t t[N_THREADS];

    for (int i = 0; i < N_THREADS; i++)
        pthread_create(&t[i], NULL, incrementer, NULL);

    for (int i = 0; i < N_THREADS; i++)
        pthread_join(t[i], NULL);

    printf("Résultat obtenu  : %ld\n", compteur);
    printf("Résultat attendu : %d\n", N_THREADS * N_ITER);

    if (compteur != (long)N_THREADS * N_ITER)
        printf("RACE CONDITION détectée !\n");
    else
        printf("Pas de race visible ce coup-ci (mais elle existe).\n");

    return 0;
}
