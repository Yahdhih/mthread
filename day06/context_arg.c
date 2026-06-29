// TP02 — Exercice 2 : Passer des arguments à un contexte
// But : créer 3 contextes, chacun reçoit un numéro et l'affiche
// Note : makecontext ne peut passer que des int en arguments directs
// Pour passer un pointeur, utiliser une variable globale ou un tableau

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE (64 * 1024)
#define N_THREADS 3

static ucontext_t contexts[N_THREADS];
static ucontext_t ctx_main;

void worker(int id) {
    printf("Thread %d démarré\n", id);
    // TODO : passer au contexte suivant, ou revenir au main si dernier
    if (id < N_THREADS - 1) {
        swapcontext(&contexts[id], &contexts[id + 1]);
    } else {
        swapcontext(&contexts[id], &ctx_main);
    }
}

int main(void) {
    // TODO : créer N_THREADS contextes, chacun exécute worker(i)
    // Enchaîner : main → thread0 → thread1 → thread2 → main
    char *stacks[N_THREADS];

    for (int i = 0; i < N_THREADS; i++) {
        stacks[i] = malloc(STACK_SIZE);
        if (!stacks[i]) { perror("malloc"); return 1; }

        getcontext(&contexts[i]);
        contexts[i].uc_stack.ss_sp   = stacks[i];
        contexts[i].uc_stack.ss_size = STACK_SIZE;
        contexts[i].uc_link          = &ctx_main;
        makecontext(&contexts[i], (void (*)(void))worker, 1, i);
    }

    // Démarrer la chaîne depuis thread 0
    swapcontext(&ctx_main, &contexts[0]);

    printf("Tous les threads ont terminé\n");

    for (int i = 0; i < N_THREADS; i++)
        free(stacks[i]);

    return 0;
}
