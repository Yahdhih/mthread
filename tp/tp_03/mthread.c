/*
 * mthread.c — Implémentation du TP 03
 *
 * CONSIGNE : Complète toutes les sections marquées TODO.
 * Ne modifie pas les signatures des fonctions.
 * Compile avec : gcc -Wall -g mthread.c test_xxx.c -o test_xxx
 */
#include "mthread.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ════════════════════════════════════════════════
   Structures internes du scheduler
   ════════════════════════════════════════════════ */

typedef struct {
    mthread_t  head;        /* tête de la run_queue (FIFO) */
    mthread_t  tail;
    mthread_t  current;     /* thread en cours d'exécution */
    ucontext_t ctx;         /* contexte du scheduler lui-même */
    int        next_id;     /* compteur d'IDs */
    int        initialized;
} scheduler_t;

static scheduler_t g_sched = {0};

/* ════════════════════════════════════════════════
   File d'attente (run queue) — liste chaînée FIFO
   ════════════════════════════════════════════════ */

static void enqueue(mthread_t t) {
    t->next = NULL;
    if (g_sched.tail)
        g_sched.tail->next = t;
    else
        g_sched.head = t;
    g_sched.tail = t;
}

static mthread_t dequeue(void) {
    if (!g_sched.head) return NULL;
    mthread_t t = g_sched.head;
    g_sched.head = t->next;
    if (!g_sched.head) g_sched.tail = NULL;
    t->next = NULL;
    return t;
}

/* ════════════════════════════════════════════════
   Wrapper de démarrage — appelé par makecontext
   ════════════════════════════════════════════════ */

/*
 * Thread argument empaqueté pour makecontext.
 * makecontext ne passe que des int, on décompose le pointeur.
 */
typedef struct {
    void *(*func)(void *);
    void  *arg;
} thread_start_t;

static void thread_entry(int hi, int lo) {
    /* Recomposer le pointeur depuis les deux int */
    uintptr_t ptr = ((uintptr_t)(unsigned int)hi << 32) | (uintptr_t)(unsigned int)lo;
    thread_start_t *start = (thread_start_t *)(void *)ptr;

    void *(*func)(void *) = start->func;
    void  *arg            = start->arg;
    free(start);

    void *retval = func(arg);

    mthread_exit(retval);
    /* mthread_exit ne revient pas — uc_link prend le relai */
}

/* ════════════════════════════════════════════════
   Boucle principale du scheduler
   ════════════════════════════════════════════════ */

static void scheduler_run(void) {
    while (1) {
        mthread_t next = dequeue();

        if (!next) {
            /* Plus rien à exécuter */
            return;
        }

        next->state     = MTHREAD_RUNNING;
        g_sched.current = next;
        swapcontext(&g_sched.ctx, &next->ctx);
        /*
         * On revient ici après :
         *   - un yield()  (thread remis en file)
         *   - un exit()   (thread en ZOMBIE)
         *   - un join()   (thread en BLOCKED)
         */
    }
}

/* ════════════════════════════════════════════════
   Initialisation
   ════════════════════════════════════════════════ */

void mthread_init(void) {
    if (g_sched.initialized) return;
    g_sched.initialized = 1;
    g_sched.next_id     = 1;
    /* Le contexte du scheduler (g_sched.ctx) sera initialisé
     * par le premier swapcontext appelé depuis main. */
}

/* ════════════════════════════════════════════════
   mthread_create
   ════════════════════════════════════════════════ */

int mthread_create(mthread_t *tid, void *(*func)(void *), void *arg) {
    mthread_init();

    /* TODO 1 : Allouer la structure mthread_struct_t */
    mthread_t t = NULL; /* remplace NULL par malloc */

    if (!t) return -1;

    /* TODO 2 : Allouer la pile */
    t->stack = NULL; /* remplace NULL par malloc(MTHREAD_STACK_SIZE) */

    if (!t->stack) { free(t); return -1; }

    /* TODO 3 : Initialiser les champs de base */
    /* t->id     = ... */
    /* t->state  = MTHREAD_READY; */
    /* t->retval = NULL; */
    /* t->joiner = NULL; */
    /* t->next   = NULL; */

    /* TODO 4 : Initialiser le contexte avec getcontext */

    /* TODO 5 : Configurer la pile */
    /* t->ctx.uc_stack.ss_sp   = t->stack; */
    /* t->ctx.uc_stack.ss_size = MTHREAD_STACK_SIZE; */

    /* TODO 6 : uc_link = contexte du scheduler (pour quand func retourne) */
    /* t->ctx.uc_link = &g_sched.ctx; */

    /* TODO 7 : Préparer les arguments pour thread_entry */
    thread_start_t *start = malloc(sizeof(*start));
    if (!start) { free(t->stack); free(t); return -1; }
    start->func = func;
    start->arg  = arg;

    /* Décomposer le pointeur en deux int pour makecontext */
    uintptr_t ptr = (uintptr_t)(void *)start;
    int hi = (int)(ptr >> 32);
    int lo = (int)(ptr & 0xFFFFFFFF);

    /* TODO 8 : makecontext pour démarrer thread_entry(hi, lo) */
    /* makecontext(&t->ctx, (void(*)(void))thread_entry, 2, hi, lo); */

    /* TODO 9 : Ajouter le thread dans la run_queue */
    /* enqueue(t); */

    *tid = t;
    return 0;
}

/* ════════════════════════════════════════════════
   mthread_yield
   ════════════════════════════════════════════════ */

void mthread_yield(void) {
    mthread_t me = g_sched.current;
    if (!me) return;

    /* TODO 10 : Changer l'état du thread courant en READY */

    /* TODO 11 : Le remettre dans la run_queue */

    /* TODO 12 : Céder le CPU au scheduler */
    /* swapcontext(&me->ctx, &g_sched.ctx); */
}

/* ════════════════════════════════════════════════
   mthread_exit
   ════════════════════════════════════════════════ */

void mthread_exit(void *retval) {
    mthread_t me = g_sched.current;

    /* TODO 13 : Sauvegarder la valeur de retour */

    /* TODO 14 : Passer en état ZOMBIE */

    /* TODO 15 : Si quelqu'un attend en join() → le réveiller (READY + enqueue) */
    if (me->joiner) {
        /* me->joiner->state = MTHREAD_READY; */
        /* enqueue(me->joiner); */
        /* me->joiner = NULL; */
    }

    /* TODO 16 : Rendre la main au scheduler SANS sauvegarder le contexte */
    /* setcontext(&g_sched.ctx); */

    /* Ne doit jamais arriver ici */
    abort();
}

/* ════════════════════════════════════════════════
   mthread_join
   ════════════════════════════════════════════════ */

int mthread_join(mthread_t tid, void **retval) {
    if (!tid) return -1;

    /* TODO 17 : Si tid est déjà ZOMBIE, récupérer retval et libérer */
    if (tid->state == MTHREAD_ZOMBIE) {
        if (retval) *retval = tid->retval;
        /* Libérer la pile et la structure */
        free(tid->stack);
        free(tid);
        return 0;
    }

    /* TODO 18 : Sinon, enregistrer le thread courant comme waiter */
    mthread_t me = g_sched.current;
    /* tid->joiner = me; */

    /* TODO 19 : Se bloquer et céder le CPU */
    /* me->state = MTHREAD_BLOCKED; */
    /* swapcontext(&me->ctx, &g_sched.ctx); */

    /* Quand on revient ici, tid est ZOMBIE */
    if (retval) *retval = tid->retval;
    free(tid->stack);
    free(tid);
    return 0;
}

/* ════════════════════════════════════════════════
   Point d'entrée : mthread_run() démarre le scheduler
   (appelé depuis main après create)
   ════════════════════════════════════════════════ */

void mthread_run(void) {
    mthread_init();
    /* Sauvegarder le contexte de main dans g_sched.ctx,
     * puis lancer la boucle scheduler_run() */
    getcontext(&g_sched.ctx);
    scheduler_run();
}
