#include "mthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Variables globales ─────────────────────────────── */
static mthread_struct_t *ready_queue = NULL;  /* tête de file READY */
static mthread_struct_t *current     = NULL;  /* thread en cours */
static ucontext_t        scheduler_ctx;       /* contexte du scheduler */
static int               next_id     = 0;
static int               scheduler_initialized = 0;

/* ── Wrapper d'entrée ───────────────────────────────── */
static void thread_entry(void) {
    /* TODO 1 : récupérer les arguments depuis current (func, arg) */
    /* TODO 2 : appeler func(arg) et récupérer la valeur de retour */
    /* TODO 3 : appeler mthread_exit() avec la valeur de retour */

    /* Indice : les champs func et arg sont dans la structure current */
    /* void *ret = current->func(current->arg); */
    /* mthread_exit(ret); */
}

/* ── File READY ─────────────────────────────────────── */
static void enqueue(mthread_struct_t *t) {
    /* TODO 4 : ajouter t en queue de ready_queue */
    /* Indice : parcourir la liste jusqu'au dernier élément (->next == NULL)
       et y attacher t. Ne pas oublier le cas où ready_queue == NULL. */
    (void)t; /* retire cette ligne quand tu implémentes */
}

static mthread_struct_t *dequeue(void) {
    /* TODO 5 : retirer et retourner la tête de ready_queue (NULL si vide) */
    /* Indice :
       mthread_struct_t *t = ready_queue;
       if (t == NULL) return NULL;
       ready_queue = t->next;
       t->next = NULL;
       return t;
    */
    return NULL;
}

/* ── Scheduler ──────────────────────────────────────── */
static void scheduler_run(void) {
    while (1) {
        mthread_struct_t *next = dequeue();
        if (next == NULL) {
            /* TODO 6 : plus rien à exécuter — sortir du scheduler */
            /* Indice : juste "return;" */
            return;
        }
        /* TODO 7 : mettre next en RUNNING et swapcontext vers lui */
        /* Indice :
           next->state = MTHREAD_RUNNING;
           current = next;
           swapcontext(&scheduler_ctx, &next->ctx);
        */
        (void)next; /* retire cette ligne quand tu implémentes */
    }
}

/* ── API publique ───────────────────────────────────── */
int mthread_create(mthread_t *thread, void *(*func)(void *), void *arg) {
    /* TODO 8 : allouer mthread_struct_t */
    /* TODO 9 : remplir id, state=READY, func/arg stockés quelque part */
    /* TODO 10 : allouer stack, configurer ucontext avec makecontext(thread_entry) */
    /* TODO 11 : enqueue le thread */
    /* TODO 12 : initialiser le scheduler_ctx si c'est la première fois */
    (void)thread; (void)func; (void)arg;
    return 0;
}

void mthread_yield(void) {
    /* TODO 13 : remettre current en READY */
    /* TODO 14 : swapcontext vers scheduler */
}

void mthread_exit(void *retval) {
    /* TODO 15 : mettre current en ZOMBIE */
    /* TODO 16 : sauvegarder retval */
    /* TODO 17 : si current->joiner != NULL, le remettre en READY */
    /* TODO 18 : setcontext vers scheduler (sans retour) */
    (void)retval;
}

int mthread_join(mthread_t thread, void **retval) {
    /* TODO 19 : si thread->state == ZOMBIE, récupérer retval et return 0 */
    /* TODO 20 : sinon, se bloquer : current->state=BLOCKED, thread->joiner=current */
    /* TODO 21 : yield vers scheduler */
    /* TODO 22 : quand réveillé, récupérer retval */
    (void)thread; (void)retval;
    return 0;
}
