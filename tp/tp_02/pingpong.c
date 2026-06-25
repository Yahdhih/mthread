/*
 * TP 02 - Exercice 1 : Ping-Pong entre deux contextes
 *
 * CONSIGNE : Complète les TODO pour faire alterner ping() et pong()
 * 5 fois chacun en utilisant uniquement swapcontext.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE (64 * 1024)

ucontext_t ctx_main, ctx_ping, ctx_pong;

void pong(void);

void ping(void) {
    for (int i = 1; i <= 5; i++) {
        printf("PING %d\n", i);
        /* TODO : céder le CPU à pong */
    }
}

void pong(void) {
    for (int i = 1; i <= 5; i++) {
        printf("PONG %d\n", i);
        /* TODO : céder le CPU à ping */
    }
}

int main(void) {
    char *stack_ping = malloc(STACK_SIZE);
    char *stack_pong = malloc(STACK_SIZE);

    if (!stack_ping || !stack_pong) {
        perror("malloc");
        return 1;
    }

    /* TODO : initialiser ctx_ping pour exécuter ping() */
    /* Indice : getcontext, puis configurer uc_stack et uc_link, puis makecontext */

    /* TODO : initialiser ctx_pong pour exécuter pong() */

    /* TODO : démarrer en activant ctx_ping depuis ctx_main */

    printf("Fin.\n");

    free(stack_ping);
    free(stack_pong);
    return 0;
}
