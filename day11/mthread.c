#include "mthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Variables globales ─────────────────────────────── */
static mthread_struct_t *ready_queue = NULL;
static mthread_struct_t *current     = NULL;
static ucontext_t        scheduler_ctx;
static int               next_id     = 0;
static int               scheduler_initialized = 0;

/* ── Wrapper d'entrée ───────────────────────────────── */
static void thread_entry(void) {
    /* TODO 1 : récupérer les arguments depuis current (func, arg)
       Les champs func et arg sont dans la structure current.
       Exemple :
         void *(*func)(void *) = current->func;
         void *arg = current->arg;
    */

    /* TODO 2 : appeler func(arg) et récupérer la valeur de retour
       Exemple :
         void *ret = func(arg);
    */

    /* TODO 3 : appeler mthread_exit() avec la valeur de retour
       Exemple :
         mthread_exit(ret);
       Attention : après mthread_exit(), on ne revient jamais ici.
    */
}

/* ── File READY ─────────────────────────────────────── */
static void enqueue(mthread_struct_t *t) {
    /* TODO 4 : ajouter t en queue de ready_queue
       Exemple :
         t->next = NULL;
         if (ready_queue == NULL) {
             ready_queue = t;
             return;
         }
         mthread_struct_t *p = ready_queue;
         while (p->next != NULL)
             p = p->next;
         p->next = t;
    */
    (void)t;
}

static mthread_struct_t *dequeue(void) {
    /* TODO 5 : retirer et retourner la tête de ready_queue (NULL si vide)
       Exemple :
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
            /* TODO 6 : plus rien à exécuter — sortir du scheduler
               Exemple :
                 return;
            */
            return;
        }
        /* TODO 7 : mettre next en RUNNING et swapcontext vers lui
           Exemple :
             next->state = MTHREAD_RUNNING;
             current = next;
             swapcontext(&scheduler_ctx, &next->ctx);
           Après le swapcontext, on revient ici au prochain yield/exit.
        */
        (void)next;
    }
}

/* ── API publique ───────────────────────────────────── */
int mthread_create(mthread_t *thread, void *(*func)(void *), void *arg) {
    /* TODO 8 : allouer mthread_struct_t
       Exemple :
         mthread_struct_t *t = calloc(1, sizeof(mthread_struct_t));
         if (!t) return -1;
    */

    /* TODO 9 : remplir id, state=READY, func, arg
       Exemple :
         t->id    = next_id++;
         t->state = MTHREAD_READY;
         t->func  = func;
         t->arg   = arg;
         t->joiner = NULL;
         t->retval = NULL;
         t->next   = NULL;
    */

    /* TODO 10 : allouer stack, configurer ucontext avec makecontext(thread_entry)
       Exemple :
         t->stack = malloc(STACK_SIZE);
         if (!t->stack) { free(t); return -1; }
         getcontext(&t->ctx);
         t->ctx.uc_stack.ss_sp   = t->stack;
         t->ctx.uc_stack.ss_size = STACK_SIZE;
         t->ctx.uc_link          = NULL;
         makecontext(&t->ctx, thread_entry, 0);
    */

    /* TODO 11 : enqueue le thread
       Exemple :
         enqueue(t);
    */

    /* TODO 12 : initialiser le scheduler_ctx si c'est la première fois
       Exemple :
         if (!scheduler_initialized) {
             getcontext(&scheduler_ctx);
             scheduler_ctx.uc_stack.ss_sp   = malloc(STACK_SIZE);
             scheduler_ctx.uc_stack.ss_size = STACK_SIZE;
             scheduler_ctx.uc_link          = NULL;
             makecontext(&scheduler_ctx, scheduler_run, 0);
             scheduler_initialized = 1;
         }
    */

    /* TODO : affecter *thread = t avant de retourner */
    (void)thread; (void)func; (void)arg;
    return 0;
}

void mthread_yield(void) {
    /* TODO 13 : remettre current en READY et l'enqueue
       Exemple :
         if (current != NULL) {
             current->state = MTHREAD_READY;
             enqueue(current);
         }
    */

    /* TODO 14 : swapcontext vers scheduler
       Exemple :
         if (current != NULL) {
             swapcontext(&current->ctx, &scheduler_ctx);
         } else {
             // main appelle yield avant tout create — cas rare
             setcontext(&scheduler_ctx);
         }
    */
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
