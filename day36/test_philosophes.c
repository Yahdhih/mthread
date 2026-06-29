#include "mthread.h"
#include "mthread_mutex.h"
#include <stdio.h>
#include <assert.h>

#define N 5
#define N_MEALS 3

static mthread_mutex_t chopsticks[N];

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left  = id;
    int right = (id + 1) % N;

    for (int meal = 0; meal < N_MEALS; meal++) {
        /* Anti-deadlock : philosophe N-1 prend droite avant gauche */
        if (id == N - 1) { int tmp=left; left=right; right=tmp; }

        mthread_mutex_lock(&chopsticks[left]);
        mthread_mutex_lock(&chopsticks[right]);

        printf("Philosophe %d mange (repas %d)\n", id, meal+1);
        mthread_yield();  /* simuler le temps de manger */

        mthread_mutex_unlock(&chopsticks[right]);
        mthread_mutex_unlock(&chopsticks[left]);

        mthread_yield();  /* penser */
    }
    return NULL;
}

int main(void) {
    for (int i = 0; i < N; i++)
        mthread_mutex_init(&chopsticks[i]);

    mthread_t philosophers[N];
    int ids[N];
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        mthread_create(&philosophers[i], philosopher, &ids[i]);
    }
    for (int i = 0; i < N; i++)
        mthread_join(philosophers[i], NULL);

    printf("[PASS] Aucun deadlock\n");
    return 0;
}
