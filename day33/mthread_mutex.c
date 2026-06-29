#include "mthread_mutex.h"
#include <stdlib.h>

int mthread_mutex_init(mthread_mutex_t *m) { m->locked=0; m->wait_queue=NULL; return 0; }

int mthread_mutex_lock(mthread_mutex_t *m) {
    if (!m->locked) { m->locked=1; return 0; }
    mthread_struct_t *me = _mthread_current();
    mthread_struct_t **p = &m->wait_queue;
    while (*p) p=&(*p)->next;
    *p=me; me->next=NULL; me->state=MTHREAD_BLOCKED;
    _mthread_schedule();
    return 0;
}

int mthread_mutex_unlock(mthread_mutex_t *m) {
    if (!m->wait_queue) { m->locked=0; return 0; }
    mthread_struct_t *next=m->wait_queue;
    m->wait_queue=next->next; next->next=NULL;
    _mthread_enqueue(next);
    return 0;
}

int mthread_mutex_trylock(mthread_mutex_t *m) {
    if (!m->locked) { m->locked=1; return 0; }
    return -1;
}

int mthread_mutex_destroy(mthread_mutex_t *m) { (void)m; return 0; }
