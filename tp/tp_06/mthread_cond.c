/*
 * mthread_cond.c — Implémentation des variables de condition
 * TP 06
 *
 * CONSIGNE : Complète les TODO. L'ordre des opérations dans cond_wait
 * est crucial : ne l'inverse pas.
 */
#include "mthread_cond.h"
#include "../tp_03/mthread_internal.h"
#include <stdlib.h>

static void cond_enqueue(mthread_cond_t *c, mthread_t t) {
    t->next = NULL;
    if (c->wait_tail) c->wait_tail->next = t;
    else              c->wait_head = t;
    c->wait_tail = t;
}

static mthread_t cond_dequeue(mthread_cond_t *c) {
    if (!c->wait_head) return NULL;
    mthread_t t = c->wait_head;
    c->wait_head = t->next;
    if (!c->wait_head) c->wait_tail = NULL;
    t->next = NULL;
    return t;
}

void mthread_cond_init(mthread_cond_t *c) {
    c->wait_head = c->wait_tail = NULL;
}

void mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m) {
    mthread_t me = g_sched.current;

    /* TODO 1 : S'inscrire dans la file d'attente de la condition */
    /* cond_enqueue(c, me); */

    /* TODO 2 : Bloquer ce thread */
    /* me->state = MTHREAD_BLOCKED; */

    /* TODO 3 : Libérer le mutex (AVANT de céder le CPU) */
    /* mthread_mutex_unlock(m); */

    /* TODO 4 : Céder le CPU au scheduler */
    /* swapcontext(&me->ctx, &g_sched.ctx); */

    /* Quand on revient ici : signal/broadcast nous a réveillé */

    /* TODO 5 : Réacquérir le mutex */
    /* mthread_mutex_lock(m); */

    (void)c; (void)m; (void)me; (void)cond_enqueue;
}

void mthread_cond_signal(mthread_cond_t *c) {
    /* TODO 6 : Réveiller le premier thread en attente (si existe) */
    /* t = cond_dequeue(c)                            */
    /* t->state = MTHREAD_READY                       */
    /* enqueue_to_run_queue(t)                         */
    (void)c; (void)cond_dequeue;
}

void mthread_cond_broadcast(mthread_cond_t *c) {
    /* TODO 7 : Réveiller TOUS les threads en attente */
    (void)c;
}
