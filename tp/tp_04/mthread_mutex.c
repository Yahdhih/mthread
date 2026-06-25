/*
 * mthread_mutex.c — Implémentation du mutex
 * TP 04
 *
 * CONSIGNE : Complète les TODO.
 * Ne pas utiliser pthread_mutex ou tout autre synchronisation externe.
 */
#include "mthread_mutex.h"
#include "../tp_03/mthread_internal.h"   /* accès à g_sched */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/* File d'attente locale au mutex */
static void mutex_enqueue(mthread_mutex_t *m, mthread_t t) {
    t->next = NULL;
    if (m->wait_tail) m->wait_tail->next = t;
    else              m->wait_head = t;
    m->wait_tail = t;
}

static mthread_t mutex_dequeue(mthread_mutex_t *m) {
    if (!m->wait_head) return NULL;
    mthread_t t = m->wait_head;
    m->wait_head = t->next;
    if (!m->wait_head) m->wait_tail = NULL;
    t->next = NULL;
    return t;
}

void mthread_mutex_init(mthread_mutex_t *m) {
    m->locked    = 0;
    m->owner     = NULL;
    m->wait_head = NULL;
    m->wait_tail = NULL;
}

void mthread_mutex_lock(mthread_mutex_t *m) {
    /* TODO 1 : Si le mutex est libre → le prendre et retourner */

    /* TODO 2 : Sinon → bloquer le thread courant */
    /*   - changer son état en BLOCKED              */
    /*   - l'ajouter dans m->wait_queue             */
    /*   - céder le CPU au scheduler                */
    /*   Quand on revient ici, le mutex nous appartient */
    (void)m;
    (void)mutex_enqueue;
}

void mthread_mutex_unlock(mthread_mutex_t *m) {
    /* TODO 3 : Vérifier que le thread courant est bien le owner */
    /* assert(m->owner == g_sched.current); */

    /* TODO 4 : Si la file d'attente est vide → libérer simplement */

    /* TODO 5 : Sinon → transférer le mutex au premier thread en attente */
    /*   - le retirer de la file                  */
    /*   - changer son état en READY              */
    /*   - m->owner = ce thread                   */
    /*   - l'ajouter dans la run_queue            */
    /*   - m->locked reste à 1                    */
    (void)m;
    (void)mutex_dequeue;
}
