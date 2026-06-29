#include "mthread_cond.h"
#include <stdlib.h>

int mthread_cond_init(mthread_cond_t *c) {
    c->wait_queue = NULL;
    return 0;
}

int mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m) {
    /* TODO 1 : récupérer le thread courant
       mthread_struct_t *me = _mthread_current();
    */

    /* TODO 2 : ajouter me à c->wait_queue (en queue)
       mthread_struct_t **p = &c->wait_queue;
       while (*p) p = &(*p)->next;
       *p = me;
       me->next  = NULL;
       me->state = MTHREAD_BLOCKED;
       (faire ceci AVANT le unlock)
    */

    /* TODO 3 : relâcher le mutex
       mthread_mutex_unlock(m);
    */

    /* TODO 4 : passer la main et dormir
       _mthread_schedule();
    */

    /* TODO 5 : réveillé → réacquérir le mutex
       mthread_mutex_lock(m);
    */

    (void)c; (void)m;
    return 0;
}

int mthread_cond_signal(mthread_cond_t *c) {
    /* TODO 6 : si c->wait_queue vide → return 0 */

    /* TODO 7 : retirer le premier thread de wait_queue
       mthread_struct_t *next = c->wait_queue;
       c->wait_queue = next->next;
       next->next = NULL;
    */

    /* TODO 8 : remettre next en READY
       _mthread_enqueue(next);
    */

    (void)c;
    return 0;
}

int mthread_cond_broadcast(mthread_cond_t *c) {
    /* TODO 9 : appeler cond_signal en boucle jusqu'à vider la queue */
    while (c->wait_queue)
        mthread_cond_signal(c);
    return 0;
}

int mthread_cond_destroy(mthread_cond_t *c) {
    (void)c;
    return 0;
}
