#include "mthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static mthread_struct_t *ready_queue = NULL;
static mthread_struct_t *current     = NULL;
static ucontext_t        sched_ctx;
static int               next_id     = 1;
static int               sched_init  = 0;

/* ── Fonctions internes ─────────────────────── */
mthread_struct_t *_mthread_current(void) { return current; }

static void enqueue(mthread_struct_t *t) {
    t->next = NULL;
    if (!ready_queue) { ready_queue = t; return; }
    mthread_struct_t *p = ready_queue;
    while (p->next) p = p->next;
    p->next = t;
}

static mthread_struct_t *dequeue(void) {
    if (!ready_queue) return NULL;
    mthread_struct_t *t = ready_queue;
    ready_queue = t->next;
    t->next = NULL;
    return t;
}

void _mthread_enqueue(mthread_struct_t *t) { enqueue(t); }

/* ── Thread entry wrapper ──────────────────── */
static void thread_entry(void) {
    void *ret = current->func(current->arg);
    mthread_exit(ret);
}

/* ── Scheduler ─────────────────────────────── */
static void scheduler_run(void) {
    while (1) {
        mthread_struct_t *next = dequeue();
        if (!next) return;
        next->state = MTHREAD_RUNNING;
        current = next;
        swapcontext(&sched_ctx, &next->ctx);
    }
}

void _mthread_schedule(void) {
    swapcontext(&current->ctx, &sched_ctx);
}

/* ── API ───────────────────────────────────── */
int mthread_create(mthread_t *out, void *(*func)(void *), void *arg) {
    mthread_struct_t *t = calloc(1, sizeof *t);
    if (!t) return -1;
    t->id    = next_id++;
    t->func  = func;
    t->arg   = arg;
    t->state = MTHREAD_READY;
    t->stack = malloc(STACK_SIZE);
    if (!t->stack) { free(t); return -1; }
    getcontext(&t->ctx);
    t->ctx.uc_stack.ss_sp   = t->stack;
    t->ctx.uc_stack.ss_size = STACK_SIZE;
    t->ctx.uc_link          = NULL;
    makecontext(&t->ctx, thread_entry, 0);
    if (!sched_init) {
        getcontext(&sched_ctx);
        sched_ctx.uc_stack.ss_sp   = malloc(STACK_SIZE);
        sched_ctx.uc_stack.ss_size = STACK_SIZE;
        sched_ctx.uc_link          = NULL;
        makecontext(&sched_ctx, scheduler_run, 0);
        sched_init = 1;
    }
    enqueue(t);
    if (out) *out = t;
    return 0;
}

void mthread_yield(void) {
    current->state = MTHREAD_READY;
    enqueue(current);
    swapcontext(&current->ctx, &sched_ctx);
}

void mthread_exit(void *retval) {
    /* TODO 15 : mettre current->state = MTHREAD_ZOMBIE */
    /* TODO 16 : sauvegarder retval dans current->retval */
    /* TODO 17 : si current->joiner != NULL, le remettre en READY avec enqueue() */
    /* TODO 18 : setcontext(&sched_ctx) pour revenir au scheduler SANS retour */
    (void)retval;
    setcontext(&sched_ctx); /* placeholder — à remplacer par le vrai code */
}

int mthread_join(mthread_t thread, void **retval) {
    /* TODO 19 : si thread->state == MTHREAD_ZOMBIE, aller à l'étape 22 */
    /* TODO 20 : sinon : current->state = MTHREAD_BLOCKED */
    /* TODO 21 :         thread->joiner = current */
    /*           puis swapcontext vers sched_ctx pour attendre */
    /* TODO 22 : quand réveillé (ou si déjà ZOMBIE) : */
    /*           si retval != NULL, *retval = thread->retval */
    /*           libérer thread->stack et thread lui-même */
    (void)thread; (void)retval;
    return 0;
}
