#ifndef MTHREAD_COND_H
#define MTHREAD_COND_H

#include "mthread.h"
#include "mthread_mutex.h"

typedef struct {
    mthread_struct_t *wait_queue;
} mthread_cond_t;

#define MTHREAD_COND_INITIALIZER { NULL }

int mthread_cond_init(mthread_cond_t *c);
int mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m);
int mthread_cond_signal(mthread_cond_t *c);
int mthread_cond_broadcast(mthread_cond_t *c);
int mthread_cond_destroy(mthread_cond_t *c);

#endif
