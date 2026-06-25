/*
 * mthread_mutex.h — Extension mutex pour mthread
 * TP 04
 */
#ifndef MTHREAD_MUTEX_H
#define MTHREAD_MUTEX_H

#include "../tp_03/mthread.h"

typedef struct {
    int        locked;
    mthread_t  owner;
    mthread_t  wait_head;
    mthread_t  wait_tail;
} mthread_mutex_t;

#define MTHREAD_MUTEX_INIT { 0, NULL, NULL, NULL }

void mthread_mutex_init(mthread_mutex_t *m);
void mthread_mutex_lock(mthread_mutex_t *m);
void mthread_mutex_unlock(mthread_mutex_t *m);

#endif
