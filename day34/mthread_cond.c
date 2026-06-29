#include "mthread_cond.h"
#include <stdlib.h>

int mthread_cond_init(mthread_cond_t *c) { c->wait_queue=NULL; return 0; }

int mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m) {
    mthread_struct_t *me=_mthread_current();
    mthread_struct_t **p=&c->wait_queue;
    while (*p) p=&(*p)->next;
    *p=me; me->next=NULL; me->state=MTHREAD_BLOCKED;
    mthread_mutex_unlock(m);
    _mthread_schedule();
    mthread_mutex_lock(m);
    return 0;
}

int mthread_cond_signal(mthread_cond_t *c) {
    /* TODO : implémenter (retirer de wait_queue, enqueue) */
    (void)c;
    return 0;
}

int mthread_cond_broadcast(mthread_cond_t *c) {
    /* TODO : réveiller tous */
    while (c->wait_queue) mthread_cond_signal(c);
    return 0;
}

int mthread_cond_timedwait(mthread_cond_t *c, mthread_mutex_t *m, int ms) {
    /* Implémentation simplifiée : ignorer le timeout dans notre modèle N:1 */
    /* (Un vrai timedwait nécessiterait un timer par thread) */
    (void)ms;
    return mthread_cond_wait(c, m);
}

int mthread_cond_destroy(mthread_cond_t *c) { (void)c; return 0; }
