/* preempt.c — Préemption par SIGALRM pour mthread
 * Compiler avec mthread.c :
 *   gcc -o prog mthread.c preempt.c prog.c
 *
 * Dans mthread_yield(), ajouter le masquage SIGALRM :
 *   voir les commentaires TODO ci-dessous
 */
#include "mthread.h"
#include <signal.h>
#include <sys/time.h>

#define QUANTUM_US 10000  /* 10 ms par thread */

static sigset_t alrm_set;

/* Appeler en DÉBUT de toute section critique de mthread */
void mthread_block_signals(void) {
    sigprocmask(SIG_BLOCK, &alrm_set, NULL);
}

/* Appeler en FIN de section critique */
void mthread_unblock_signals(void) {
    sigprocmask(SIG_UNBLOCK, &alrm_set, NULL);
}

static void sigalrm_handler(int sig) {
    (void)sig;
    /* TODO 1 : appeler mthread_yield() */
    /* Note : mthread_yield() masquera SIGALRM lui-même,
     * donc pas de recursion infinie */
}

void mthread_preempt_enable(void) {
    /* TODO 2 : initialiser alrm_set */
    sigemptyset(&alrm_set);
    sigaddset(&alrm_set, SIGALRM);

    /* TODO 3 : installer le handler SIGALRM avec sigaction */
    struct sigaction sa;
    /* sa.sa_handler = sigalrm_handler; */
    /* sa.sa_flags   = SA_RESTART; */
    /* sigemptyset(&sa.sa_mask); */
    /* sigaction(SIGALRM, &sa, NULL); */
    (void)sa;

    /* TODO 4 : démarrer le timer (répétitif toutes les QUANTUM_US) */
    struct itimerval timer;
    timer.it_value.tv_sec     = 0;
    timer.it_value.tv_usec    = QUANTUM_US;
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_usec = QUANTUM_US;
    /* setitimer(ITIMER_REAL, &timer, NULL); */
    (void)timer;
}

void mthread_preempt_disable(void) {
    /* TODO 5 : arrêter le timer (passer des zéros à setitimer) */
    struct itimerval zero = {{0,0},{0,0}};
    /* setitimer(ITIMER_REAL, &zero, NULL); */
    (void)zero;
}

/* ─────────────────────────────────────────────────────────────────
 * Modification à faire dans mthread.c → mthread_yield() :
 *
 * void mthread_yield(void) {
 *     mthread_block_signals();       // ← ajouter
 *     current->state = MTHREAD_READY;
 *     enqueue(current);
 *     swapcontext(&current->ctx, &sched_ctx);
 *     mthread_unblock_signals();     // ← ajouter
 * }
 *
 * Aussi dans scheduler_run() → swapcontext() :
 * masquer avant, démasquer après.
 * ───────────────────────────────────────────────────────────────── */
