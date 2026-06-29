// TP02 — Exercice 3 : Chaîne circulaire de N contextes
// But : N threads se passent un compteur. Quand le compteur atteint MAX,
//       le dernier thread retourne au main.

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE (64 * 1024)
#define N 4
#define MAX_COUNT 10

static ucontext_t threads[N];
static ucontext_t main_ctx;
static int counter = 0;

void thread_func(int id) {
    while (1) {
        counter++;
        printf("Thread %d : counter = %d\n", id, counter);
        if (counter >= MAX_COUNT) {
            // TODO : retourner au main
            swapcontext(&threads[id], &main_ctx);
            return;
        }
        // TODO : passer au thread suivant (circulaire)
        int next = (id + 1) % N;
        swapcontext(&threads[id], &threads[next]);
    }
}

int main(void) {
    // TODO : initialiser les N contextes
    char *stacks[N];
    for (int i = 0; i < N; i++) {
        stacks[i] = malloc(STACK_SIZE);
        if (!stacks[i]) { perror("malloc"); return 1; }

        getcontext(&threads[i]);
        threads[i].uc_stack.ss_sp   = stacks[i];
        threads[i].uc_stack.ss_size = STACK_SIZE;
        threads[i].uc_link          = &main_ctx;
        makecontext(&threads[i], (void (*)(void))thread_func, 1, i);
    }

    // TODO : démarrer avec le thread 0
    swapcontext(&main_ctx, &threads[0]);

    printf("Counter final : %d\n", counter);

    for (int i = 0; i < N; i++)
        free(stacks[i]);
    return 0;
}
