/*
 * mthread_cond.h — Variables de condition pour mthread
 * TP 06
 */
#ifndef MTHREAD_COND_H
#define MTHREAD_COND_H

#include "../tp_03/mthread.h"
#include "../tp_04/mthread_mutex.h"

typedef struct {
    mthread_t wait_head;
    mthread_t wait_tail;
} mthread_cond_t;

#define MTHREAD_COND_INIT { NULL, NULL }

void mthread_cond_init(mthread_cond_t *c);
void mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m);
void mthread_cond_signal(mthread_cond_t *c);
void mthread_cond_broadcast(mthread_cond_t *c);

#endif
