#ifndef MTHREAD_SEM_H
#define MTHREAD_SEM_H

#include "mthread.h"

typedef struct {
    int value;
    mthread_struct_t *wait_queue;
} mthread_sem_t;

int mthread_sem_init(mthread_sem_t *s, int value);
int mthread_sem_wait(mthread_sem_t *s);
int mthread_sem_post(mthread_sem_t *s);
int mthread_sem_destroy(mthread_sem_t *s);

#endif
