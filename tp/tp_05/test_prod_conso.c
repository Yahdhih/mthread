/*
 * test_prod_conso.c — Producteur/Consommateur avec sémaphores
 *
 * Sortie attendue (ordre peut varier) :
 *   Produit : 0
 *   Consommé : 0
 *   Produit : 1
 *   Consommé : 1
 *   ...
 *   Produit : 9
 *   Consommé : 9
 */
#include "mthread_sem.h"
#include "../tp_03/mthread.h"
#include <stdio.h>

#define BUFFER_SIZE  5
#define N_ITEMS      10

int    buffer[BUFFER_SIZE];
int    in  = 0;
int    out = 0;

mthread_sem_t empty;   /* cases vides disponibles */
mthread_sem_t full;    /* cases pleines disponibles */
mthread_sem_t mutex;   /* accès exclusif au buffer */

void *producteur(void *arg) {
    (void)arg;
    for (int i = 0; i < N_ITEMS; i++) {
        mthread_sem_wait(&empty);   /* attendre une case vide */
        mthread_sem_wait(&mutex);   /* section critique */

        buffer[in] = i;
        in = (in + 1) % BUFFER_SIZE;
        printf("Produit : %d\n", i);

        mthread_sem_post(&mutex);
        mthread_sem_post(&full);    /* signaler une case pleine */
    }
    return NULL;
}

void *consommateur(void *arg) {
    (void)arg;
    for (int i = 0; i < N_ITEMS; i++) {
        mthread_sem_wait(&full);    /* attendre une case pleine */
        mthread_sem_wait(&mutex);

        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        printf("Consommé : %d\n", item);

        mthread_sem_post(&mutex);
        mthread_sem_post(&empty);
    }
    return NULL;
}

int main(void) {
    mthread_sem_init(&empty, BUFFER_SIZE);
    mthread_sem_init(&full,  0);
    mthread_sem_init(&mutex, 1);

    mthread_t tp, tc;
    mthread_create(&tp, producteur, NULL);
    mthread_create(&tc, consommateur, NULL);

    mthread_run();

    mthread_join(tp, NULL);
    mthread_join(tc, NULL);

    printf("Test terminé.\n");
    return 0;
}
