/*
 * TP 01 - observer.c
 * Lance 4 threads qui dorment 30 secondes.
 * Utilise-le pour observer les threads avec ps, top, /proc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *worker(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d démarré (TID système visible dans ps -eLf)\n", id);
    sleep(30);
    return NULL;
}

int main(void) {
    pthread_t threads[4];
    int ids[4];

    printf("PID du processus : %d\n", getpid());
    printf("Crée 4 threads...\n");

    for (int i = 0; i < 4; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    printf("Tous démarrés. Lance dans un autre terminal :\n");
    printf("  ps -eLf | grep observer\n");
    printf("  top -H -p %d\n", getpid());
    printf("  ls /proc/%d/task/\n\n", getpid());

    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
