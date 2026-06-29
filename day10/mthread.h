#ifndef MTHREAD_H
#define MTHREAD_H

#include <ucontext.h>

#define STACK_SIZE (64 * 1024)

typedef enum {
    MTHREAD_READY,
    MTHREAD_RUNNING,
    MTHREAD_BLOCKED,
    MTHREAD_ZOMBIE
} mthread_state_t;

typedef struct mthread_struct {
    int              id;
    ucontext_t       ctx;
    char            *stack;
    mthread_state_t  state;
    void            *(*func)(void *);
    void            *arg;
    void            *retval;
    struct mthread_struct *joiner;
    struct mthread_struct *next;
} mthread_struct_t;

typedef mthread_struct_t *mthread_t;

int   mthread_create(mthread_t *thread, void *(*func)(void *), void *arg);
void  mthread_yield(void);
int   mthread_join(mthread_t thread, void **retval);
void  mthread_exit(void *retval);

#endif
