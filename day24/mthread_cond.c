#include "mthread_cond.h"
#include <stdlib.h>

int mthread_cond_init(mthread_cond_t *c) { c->wait_queue=NULL; return 0; }

/* cond_wait — déjà implémenté */
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
    /* TODO 6 : si c->wait_queue vide → return 0 */
    /* TODO 7 : retirer le premier thread de c->wait_queue */
    /* TODO 8 : appeler _mthread_enqueue(next) */
    (void)c;
    return 0;
}

int mthread_cond_broadcast(mthread_cond_t *c) {
    /* TODO 9 : appeler cond_signal en boucle jusqu'à vider la queue */
    while (c->wait_queue) mthread_cond_signal(c);
    return 0;
}

int mthread_cond_destroy(mthread_cond_t *c) { (void)c; return 0; }
