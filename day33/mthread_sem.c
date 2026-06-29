#include "mthread_sem.h"
#include <stdlib.h>

int mthread_sem_init(mthread_sem_t *s, int value) { s->value=value; s->wait_queue=NULL; return 0; }

int mthread_sem_wait(mthread_sem_t *s) {
    if (s->value>0) { s->value--; return 0; }
    mthread_struct_t *me=_mthread_current();
    mthread_struct_t **p=&s->wait_queue;
    while (*p) p=&(*p)->next;
    *p=me; me->next=NULL; me->state=MTHREAD_BLOCKED;
    _mthread_schedule();
    return 0;
}

int mthread_sem_post(mthread_sem_t *s) {
    if (!s->wait_queue) { s->value++; return 0; }
    mthread_struct_t *next=s->wait_queue;
    s->wait_queue=next->next; next->next=NULL;
    _mthread_enqueue(next);
    return 0;
}

int mthread_sem_trywait(mthread_sem_t *s) {
    if (s->value>0) { s->value--; return 0; }
    return -1;
}

int mthread_sem_destroy(mthread_sem_t *s) { (void)s; return 0; }
