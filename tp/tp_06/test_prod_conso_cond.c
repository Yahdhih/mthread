/*
 * test_prod_conso_cond.c — Producteur/Consommateur avec mutex + cond
 *
 * Même sémantique que le TP05 mais implémenté avec variables de condition.
 * Sortie attendue : Produit 0..9, Consommé 0..9 (ordre entrelacé)
 */
#include "mthread_cond.h"
#include "../tp_03/mthread.h"
#include <stdio.h>

#define CAP      5
#define N_ITEMS  10

int buffer[CAP];
int count = 0;

mthread_mutex_t mutex    = MTHREAD_MUTEX_INIT;
mthread_cond_t  non_plein = MTHREAD_COND_INIT;
mthread_cond_t  non_vide  = MTHREAD_COND_INIT;

void *producteur(void *arg) {
    (void)arg;
    for (int i = 0; i < N_ITEMS; i++) {
        mthread_mutex_lock(&mutex);
        while (count == CAP)
            mthread_cond_wait(&non_plein, &mutex);

        buffer[count++] = i;
        printf("Produit  : %d  (count=%d)\n", i, count);

        mthread_cond_signal(&non_vide);
        mthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consommateur(void *arg) {
    (void)arg;
    for (int i = 0; i < N_ITEMS; i++) {
        mthread_mutex_lock(&mutex);
        while (count == 0)
            mthread_cond_wait(&non_vide, &mutex);

        int item = buffer[--count];
        printf("Consommé : %d  (count=%d)\n", item, count);

        mthread_cond_signal(&non_plein);
        mthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void) {
    mthread_t tp, tc;
    mthread_create(&tp, producteur, NULL);
    mthread_create(&tc, consommateur, NULL);

    mthread_run();

    mthread_join(tp, NULL);
    mthread_join(tc, NULL);

    printf("Test terminé.\n");
    return 0;
}
