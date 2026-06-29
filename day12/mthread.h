#ifndef MTHREAD_H
#define MTHREAD_H
#include <ucontext.h>
#define STACK_SIZE (64 * 1024)
typedef enum { MTHREAD_READY, MTHREAD_RUNNING, MTHREAD_BLOCKED, MTHREAD_ZOMBIE } mthread_state_t;
typedef struct mthread_struct {
    int id; ucontext_t ctx; char *stack; mthread_state_t state;
    void *(*func)(void *); void *arg; void *retval;
    struct mthread_struct *joiner; struct mthread_struct *next;
} mthread_struct_t;
typedef mthread_struct_t *mthread_t;
int  mthread_create(mthread_t *t, void *(*func)(void *), void *arg);
void mthread_yield(void);
void mthread_exit(void *retval);
int  mthread_join(mthread_t t, void **retval);
/* Fonctions internes exposées pour mutex/sem/cond */
mthread_struct_t *_mthread_current(void);
void _mthread_enqueue(mthread_struct_t *t);
void _mthread_schedule(void);
#endif
