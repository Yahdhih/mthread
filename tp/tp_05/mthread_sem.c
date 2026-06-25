/*
 * mthread_sem.c — Implémentation des sémaphores
 * TP 05
 *
 * CONSIGNE : Complète les TODO.
 */
#include "mthread_sem.h"
#include "../tp_03/mthread_internal.h"
#include <assert.h>
#include <stdlib.h>

static void sem_enqueue(mthread_sem_t *s, mthread_t t) {
    t->next = NULL;
    if (s->wait_tail) s->wait_tail->next = t;
    else              s->wait_head = t;
    s->wait_tail = t;
}

static mthread_t sem_dequeue(mthread_sem_t *s) {
    if (!s->wait_head) return NULL;
    mthread_t t = s->wait_head;
    s->wait_head = t->next;
    if (!s->wait_head) s->wait_tail = NULL;
    t->next = NULL;
    return t;
}

void mthread_sem_init(mthread_sem_t *s, int value) {
    assert(value >= 0);
    /* TODO : initialiser les champs */
    (void)s; (void)value;
}

void mthread_sem_wait(mthread_sem_t *s) {
    /* TODO : décrémenter ou bloquer */
    (void)s; (void)sem_enqueue;
}

void mthread_sem_post(mthread_sem_t *s) {
    /* TODO : incrémenter ou réveiller */
    (void)s; (void)sem_dequeue;
}
