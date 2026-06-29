#ifndef MTHREAD_MUTEX_H
#define MTHREAD_MUTEX_H

#include "mthread.h"

typedef struct {
    int locked;
    mthread_struct_t *wait_queue;
} mthread_mutex_t;

#define MTHREAD_MUTEX_INITIALIZER { 0, NULL }

int mthread_mutex_init(mthread_mutex_t *m);
int mthread_mutex_lock(mthread_mutex_t *m);
int mthread_mutex_unlock(mthread_mutex_t *m);
int mthread_mutex_trylock(mthread_mutex_t *m);
int mthread_mutex_destroy(mthread_mutex_t *m);

#endif
