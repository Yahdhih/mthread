#include "mthread_mutex.h"
#include <stdlib.h>

int mthread_mutex_init(mthread_mutex_t *m) {
    m->locked     = 0;
    m->wait_queue = NULL;
    return 0;
}

int mthread_mutex_lock(mthread_mutex_t *m) {
    /* TODO 1 : si m->locked == 0, verrouiller et retourner
       m->locked = 1;
       return 0;
    */

    /* TODO 2 : récupérer le thread courant et l'ajouter à m->wait_queue
       mthread_struct_t *me = _mthread_current();
       me->state = MTHREAD_BLOCKED;
       // ajouter me en queue de m->wait_queue (parcourir jusqu'au bout)
       mthread_struct_t **p = &m->wait_queue;
       while (*p) p = &(*p)->next;
       *p = me;
       me->next = NULL;
    */

    /* TODO 3 : passer la main au scheduler
       _mthread_schedule();
       return 0;   ← réveillé ici avec le lock acquis
    */

    (void)m;
    return 0;
}

int mthread_mutex_unlock(mthread_mutex_t *m) {
    /* TODO 4 : si wait_queue vide → m->locked = 0 et retourner */

    /* TODO 5 : sinon, retirer le premier thread de wait_queue
       mthread_struct_t *next = m->wait_queue;
       m->wait_queue = next->next;
       next->next = NULL;
    */

    /* TODO 6 : remettre next en READY (ne PAS changer m->locked)
       next->state = MTHREAD_READY;
       _mthread_enqueue(next);
    */

    (void)m;
    return 0;
}

int mthread_mutex_trylock(mthread_mutex_t *m) {
    /* TODO 7 : si m->locked == 0 → verrouiller et retourner 0
                sinon             → retourner -1 sans bloquer
    */
    (void)m;
    return -1;
}

int mthread_mutex_destroy(mthread_mutex_t *m) {
    (void)m;
    return 0;
}
