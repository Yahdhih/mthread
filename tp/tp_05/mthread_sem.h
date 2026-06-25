/*
 * mthread_sem.h — Sémaphores pour mthread
 * TP 05
 */
#ifndef MTHREAD_SEM_H
#define MTHREAD_SEM_H

#include "../tp_03/mthread.h"

typedef struct {
    int        value;
    mthread_t  wait_head;
    mthread_t  wait_tail;
} mthread_sem_t;

void mthread_sem_init(mthread_sem_t *s, int value);
void mthread_sem_wait(mthread_sem_t *s);
void mthread_sem_post(mthread_sem_t *s);

#endif
