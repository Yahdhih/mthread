/* benchmark.c — Comparer mthread vs pthread
 *
 * Compiler pour mthread :
 *   gcc -o bench_mthread benchmark.c mthread.c mthread_mutex.c -DUSE_MTHREAD
 *
 * Compiler pour pthread :
 *   gcc -o bench_pthread benchmark.c -lpthread -DUSE_PTHREAD
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef USE_MTHREAD
#  include "mthread.h"
#  include "mthread_mutex.h"
#  define THREAD_T    mthread_t
#  define MUTEX_T     mthread_mutex_t
#  define THR_CREATE(t,f,a) mthread_create(&(t),(f),(a))
#  define THR_JOIN(t)        mthread_join((t),NULL)
#  define MUTEX_LOCK(m)      mthread_mutex_lock(&(m))
#  define MUTEX_UNLOCK(m)    mthread_mutex_unlock(&(m))
#  define MUTEX_INIT(m)      mthread_mutex_init(&(m))
#elif defined(USE_PTHREAD)
#  include <pthread.h>
#  define THREAD_T    pthread_t
#  define MUTEX_T     pthread_mutex_t
#  define THR_CREATE(t,f,a) pthread_create(&(t),NULL,(f),(a))
#  define THR_JOIN(t)        pthread_join((t),NULL)
#  define MUTEX_LOCK(m)      pthread_mutex_lock(&(m))
#  define MUTEX_UNLOCK(m)    pthread_mutex_unlock(&(m))
#  define MUTEX_INIT(m)      pthread_mutex_init(&(m),NULL)
#else
#  error "Définir USE_MTHREAD ou USE_PTHREAD"
#endif

static double now_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

static void *empty_func(void *a) { (void)a; return NULL; }

/* Bench 1 : création de N threads */
static void bench_create(int n) {
    THREAD_T *threads = malloc(n * sizeof(THREAD_T));
    double t0 = now_ms();
    for (int i = 0; i < n; i++) THR_CREATE(threads[i], empty_func, NULL);
    for (int i = 0; i < n; i++) THR_JOIN(threads[i]);
    double t1 = now_ms();
    printf("create %4d threads : %7.2f ms  (%.2f µs/thread)\n",
           n, t1-t0, (t1-t0)*1000/n);
    free(threads);
}

/* Bench 2 : mutex contention */
static MUTEX_T bench_mu;
static long bench_counter = 0;

static void *mutex_worker(void *arg) {
    int n = *(int *)arg;
    for (int i = 0; i < n; i++) {
        MUTEX_LOCK(bench_mu);
        bench_counter++;
        MUTEX_UNLOCK(bench_mu);
    }
    return NULL;
}

static void bench_mutex(int n_threads, int n_iter) {
    MUTEX_INIT(bench_mu);
    bench_counter = 0;
    THREAD_T *threads = malloc(n_threads * sizeof(THREAD_T));
    double t0 = now_ms();
    for (int i = 0; i < n_threads; i++)
        THR_CREATE(threads[i], mutex_worker, &n_iter);
    for (int i = 0; i < n_threads; i++)
        THR_JOIN(threads[i]);
    double t1 = now_ms();
    printf("mutex %d threads × %d iter : %.2f ms\n", n_threads, n_iter, t1-t0);
    free(threads);
}

int main(void) {
#ifdef USE_MTHREAD
    printf("=== Benchmark MTHREAD ===\n");
#else
    printf("=== Benchmark PTHREAD ===\n");
#endif
    bench_create(100);
    bench_create(1000);
    bench_mutex(4, 25000);
    return 0;
}
