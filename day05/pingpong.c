// TP02 — Exercice 1 : Ping-Pong entre deux contextes
// But : faire alterner deux contextes qui affichent "Ping!" et "Pong!"
// Expected output :
//   Ping!
//   Pong!
//   Ping!
//   Pong!
//   ... (5 fois chacun)
//   Done.

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE (64 * 1024)
#define N_ROUNDS 5

static ucontext_t ctx_ping, ctx_pong, ctx_main;

void ping(void) {
    for (int i = 0; i < N_ROUNDS; i++) {
        printf("Ping!\n");
        // TODO : passer la main à pong
        swapcontext(&ctx_ping, &ctx_pong);
    }
    // TODO : retourner au contexte principal
    swapcontext(&ctx_ping, &ctx_main);
}

void pong(void) {
    for (int i = 0; i < N_ROUNDS; i++) {
        printf("Pong!\n");
        // TODO : passer la main à ping
        swapcontext(&ctx_pong, &ctx_ping);
    }
    // TODO : retourner au contexte principal
    swapcontext(&ctx_pong, &ctx_main);
}

int main(void) {
    // TODO 1 : allouer une pile pour ctx_ping
    char *stack_ping = malloc(STACK_SIZE);
    if (!stack_ping) { perror("malloc"); return 1; }

    // TODO 2 : initialiser ctx_ping avec getcontext
    getcontext(&ctx_ping);

    // TODO 3 : configurer ctx_ping.uc_stack et makecontext(ping)
    ctx_ping.uc_stack.ss_sp   = stack_ping;
    ctx_ping.uc_stack.ss_size = STACK_SIZE;
    ctx_ping.uc_link          = &ctx_main;
    makecontext(&ctx_ping, ping, 0);

    // TODO 4 : faire pareil pour ctx_pong
    char *stack_pong = malloc(STACK_SIZE);
    if (!stack_pong) { perror("malloc"); free(stack_ping); return 1; }

    getcontext(&ctx_pong);
    ctx_pong.uc_stack.ss_sp   = stack_pong;
    ctx_pong.uc_stack.ss_size = STACK_SIZE;
    ctx_pong.uc_link          = &ctx_main;
    makecontext(&ctx_pong, pong, 0);

    // TODO 5 : démarrer la chaîne avec swapcontext(&ctx_main, &ctx_ping)
    swapcontext(&ctx_main, &ctx_ping);

    printf("Done.\n");

    free(stack_ping);
    free(stack_pong);
    return 0;
}
