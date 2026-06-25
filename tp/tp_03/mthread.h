/*
 * mthread.h — Interface publique de la bibliothèque mthread
 * TP 03 : implémenter create, yield, join, exit
 */
#ifndef MTHREAD_H
#define MTHREAD_H

#include <ucontext.h>
#include <stddef.h>

/* ── Taille de pile par thread ── */
#define MTHREAD_STACK_SIZE (64 * 1024)   /* 64 Ko */

/* ── États d'un thread ── */
typedef enum {
    MTHREAD_READY,
    MTHREAD_RUNNING,
    MTHREAD_BLOCKED,
    MTHREAD_ZOMBIE
} mthread_state_t;

/* ── Structure d'un thread ── */
typedef struct mthread_struct {
    int                   id;
    ucontext_t            ctx;
    char                 *stack;
    mthread_state_t       state;
    void                 *retval;
    struct mthread_struct *joiner;  /* thread qui attend en join() sur celui-ci */
    struct mthread_struct *next;    /* maillon de liste chaînée */
} mthread_struct_t;

typedef mthread_struct_t *mthread_t;

/* ── API publique ── */
int  mthread_create(mthread_t *tid, void *(*func)(void *), void *arg);
void mthread_yield(void);
int  mthread_join(mthread_t tid, void **retval);
void mthread_exit(void *retval);

/* ── Initialisation (appelée automatiquement) ── */
void mthread_init(void);

#endif /* MTHREAD_H */
