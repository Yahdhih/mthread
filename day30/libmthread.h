/* libmthread.h — API publique complète de la bibliothèque mthread
 * Cette version finale couvre toutes les fonctions du projet.
 */
#ifndef LIBMTHREAD_H
#define LIBMTHREAD_H

#include <ucontext.h>

#define STACK_SIZE (64 * 1024)

/* ── Types de base ─────────────────────────────────── */
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
    void *(*func)(void *);
    void            *arg;
    void            *retval;
    int              priority;        /* pour le scheduler à priorités */
    struct mthread_struct *joiner;
    struct mthread_struct *next;
} mthread_struct_t;

typedef mthread_struct_t *mthread_t;

/* ── Mutex ─────────────────────────────────────────── */
typedef struct {
    int              locked;
    mthread_struct_t *wait_queue;
    mthread_struct_t *owner;         /* pour l'héritage de priorité */
} mthread_mutex_t;

#define MTHREAD_MUTEX_INITIALIZER { 0, NULL, NULL }

/* ── Sémaphore ─────────────────────────────────────── */
typedef struct {
    int              value;
    mthread_struct_t *wait_queue;
} mthread_sem_t;

/* ── Variable de condition ─────────────────────────── */
typedef struct {
    mthread_struct_t *wait_queue;
} mthread_cond_t;

#define MTHREAD_COND_INITIALIZER { NULL }

/* ═══════════════════════════════════════════════════
 * API — Threads
 * ═══════════════════════════════════════════════════ */
int  mthread_create(mthread_t *t, void *(*func)(void *), void *arg);
void mthread_yield(void);
void mthread_exit(void *retval);
int  mthread_join(mthread_t t, void **retval);

/* ═══════════════════════════════════════════════════
 * API — Mutex
 * ═══════════════════════════════════════════════════ */
int mthread_mutex_init(mthread_mutex_t *m);
int mthread_mutex_lock(mthread_mutex_t *m);
int mthread_mutex_unlock(mthread_mutex_t *m);
int mthread_mutex_trylock(mthread_mutex_t *m);      /* 0=ok, -1=busy */
int mthread_mutex_destroy(mthread_mutex_t *m);

/* ═══════════════════════════════════════════════════
 * API — Sémaphores
 * ═══════════════════════════════════════════════════ */
int mthread_sem_init(mthread_sem_t *s, int value);
int mthread_sem_wait(mthread_sem_t *s);
int mthread_sem_post(mthread_sem_t *s);
int mthread_sem_trywait(mthread_sem_t *s);          /* 0=ok, -1=busy */
int mthread_sem_destroy(mthread_sem_t *s);

/* ═══════════════════════════════════════════════════
 * API — Variables de Condition
 * ═══════════════════════════════════════════════════ */
int mthread_cond_init(mthread_cond_t *c);
int mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m);
int mthread_cond_signal(mthread_cond_t *c);
int mthread_cond_broadcast(mthread_cond_t *c);
int mthread_cond_timedwait(mthread_cond_t *c, mthread_mutex_t *m, int ms);
int mthread_cond_destroy(mthread_cond_t *c);

/* ═══════════════════════════════════════════════════
 * API — Préemption
 * ═══════════════════════════════════════════════════ */
void mthread_preempt_enable(void);
void mthread_preempt_disable(void);

/* ═══════════════════════════════════════════════════
 * Fonctions internes (utilisées par mutex/sem/cond)
 * ═══════════════════════════════════════════════════ */
mthread_struct_t *_mthread_current(void);
void _mthread_enqueue(mthread_struct_t *t);
void _mthread_schedule(void);

#endif /* LIBMTHREAD_H */
