/*
 * TP 02 - Exercice 2 : Passer un pointeur à makecontext
 *
 * makecontext ne prend que des int. Pour passer un void* (64 bits sur x86_64),
 * il faut le décomposer en deux int (partie haute et partie basse).
 *
 * CONSIGNE : Complète ptr_to_ints() et ints_to_ptr(), puis utilise-les
 * dans main pour passer la chaîne "Bonjour depuis le contexte !" à worker().
 */
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <stdint.h>

#define STACK_SIZE (64 * 1024)

ucontext_t ctx_main, ctx_worker;

/* Décompose un pointeur en deux int */
void ptr_to_ints(void *ptr, int *hi, int *lo) {
    /* TODO */
    (void)ptr; (void)hi; (void)lo;
}

/* Recompose un pointeur depuis deux int */
void *ints_to_ptr(int hi, int lo) {
    /* TODO */
    (void)hi; (void)lo;
    return NULL;
}

/* Reçoit un pointeur vers une chaîne via deux int */
void worker(int hi, int lo) {
    char *msg = (char *)ints_to_ptr(hi, lo);
    printf("Worker reçoit : '%s'\n", msg);
    swapcontext(&ctx_worker, &ctx_main);
}

int main(void) {
    char *message = "Bonjour depuis le contexte !";
    char *stack = malloc(STACK_SIZE);

    getcontext(&ctx_worker);
    ctx_worker.uc_stack.ss_sp   = stack;
    ctx_worker.uc_stack.ss_size = STACK_SIZE;
    ctx_worker.uc_link          = &ctx_main;

    int hi, lo;
    ptr_to_ints(message, &hi, &lo);

    /* TODO : makecontext avec worker, 2 arguments (hi, lo) */

    swapcontext(&ctx_main, &ctx_worker);

    printf("Retour dans main.\n");
    free(stack);
    return 0;
}
