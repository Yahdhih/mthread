#include "mthread_sem.h"
#include <stdlib.h>

int mthread_sem_init(mthread_sem_t *s, int value) {
    /* TODO 1 : s->value = value; s->wait_queue = NULL; */
    (void)s; (void)value;
    return 0;
}

int mthread_sem_wait(mthread_sem_t *s) {
    /* TODO 2 : si s->value > 0, décrémenter et retourner 0 */

    /* TODO 3 : sinon, ajouter le thread courant à s->wait_queue
       mthread_struct_t *me = _mthread_current();
       mthread_struct_t **p = &s->wait_queue;
       while (*p) p = &(*p)->next;
       *p = me;
       me->next = NULL;
       me->state = MTHREAD_BLOCKED;
    */

    /* TODO 4 : appeler _mthread_schedule() pour passer la main */

    (void)s;
    return 0;
}

int mthread_sem_post(mthread_sem_t *s) {
    /* TODO 5 : si s->wait_queue vide, s->value++ et retourner 0 */

    /* TODO 6 : sinon, retirer le premier thread de wait_queue
       mthread_struct_t *next = s->wait_queue;
       s->wait_queue = next->next;
       next->next = NULL;
    */

    /* TODO 7 : remettre next en READY avec _mthread_enqueue()
       (NE PAS incrémenter s->value) */

    (void)s;
    return 0;
}

int mthread_sem_destroy(mthread_sem_t *s) {
    (void)s;
    return 0;
}
