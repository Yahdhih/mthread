# Projet Final — Bibliothèque `mthread` complète

> Durée estimée : 2 à 3 semaines.
> Prérequis : tous les TP (01 à 07) complétés et compris.

---

## Objectif

Produire une bibliothèque C propre, testée, et documentée qui réimplémente l'essentiel de `pthread` en mode utilisateur (N:1). Elle sera utilisable avec une API quasi-identique à `pthread`.

---

## API finale à livrer

```c
/* ── Threads ── */
int  mthread_create(mthread_t *t, void *(*f)(void *), void *arg);
void mthread_yield(void);
int  mthread_join(mthread_t t, void **retval);
void mthread_exit(void *retval);

/* ── Mutex ── */
int  mthread_mutex_init(mthread_mutex_t *m, const mthread_mutexattr_t *attr);
int  mthread_mutex_lock(mthread_mutex_t *m);
int  mthread_mutex_trylock(mthread_mutex_t *m);    ← NOUVEAU
int  mthread_mutex_unlock(mthread_mutex_t *m);
int  mthread_mutex_destroy(mthread_mutex_t *m);    ← NOUVEAU

/* ── Sémaphores ── */
int  mthread_sem_init(mthread_sem_t *s, int value);
int  mthread_sem_wait(mthread_sem_t *s);
int  mthread_sem_trywait(mthread_sem_t *s);        ← NOUVEAU
int  mthread_sem_post(mthread_sem_t *s);
int  mthread_sem_getvalue(mthread_sem_t *s, int *val); ← NOUVEAU

/* ── Variables de condition ── */
int  mthread_cond_init(mthread_cond_t *c, const mthread_condattr_t *attr);
int  mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m);
int  mthread_cond_timedwait(mthread_cond_t *c, mthread_mutex_t *m,
                             const struct timespec *timeout); ← NOUVEAU
int  mthread_cond_signal(mthread_cond_t *c);
int  mthread_cond_broadcast(mthread_cond_t *c);
int  mthread_cond_destroy(mthread_cond_t *c);

/* ── Scheduler ── */
void mthread_set_scheduler(int algo);  /* MTHREAD_SCHED_FIFO ou MTHREAD_SCHED_RR */
void mthread_set_quantum(int ms);
```

---

## Structure du projet

```
projet/
├── include/
│   └── mthread.h          ← interface publique complète
├── src/
│   ├── scheduler.c        ← FIFO + Round-Robin + préemption
│   ├── thread.c           ← create / join / exit / yield
│   ├── mutex.c            ← lock / trylock / unlock / destroy
│   ├── semaphore.c        ← wait / trywait / post / getvalue
│   └── cond.c             ← wait / timedwait / signal / broadcast
├── tests/
│   ├── test_threads.c
│   ├── test_mutex.c
│   ├── test_sem.c
│   ├── test_cond.c
│   ├── test_stress.c      ← test de charge (1000 threads)
│   └── test_philosophes.c ← dîner des philosophes
├── Makefile
└── README.md
```

---

## Livrable 1 — Bibliothèque statique

```bash
make lib        # produit libmthread.a
gcc -o mon_prog mon_prog.c -L. -lmthread
```

---

## Livrable 2 — Suite de tests

```bash
make test       # doit passer 100% des tests
```

Chaque test produit : `[PASS]` ou `[FAIL] : raison`.

### Tests obligatoires

| Test                 | Ce qu'il vérifie                               |
|----------------------|------------------------------------------------|
| `test_create_join`   | Création + jointure, valeur de retour          |
| `test_yield_order`   | Ordre FIFO du scheduler                        |
| `test_mutex_counter` | Compteur partagé : résultat exact              |
| `test_mutex_deadlock_detect` | Double lock → message d'erreur propre |
| `test_sem_producer`  | Producteur/consommateur : toutes les données   |
| `test_cond_broadcast`| Broadcast réveille tous les threads            |
| `test_stress`        | 500 threads, 10 000 itérations, pas de crash   |
| `test_philosophes`   | Pas de deadlock en 10 secondes                 |
| `test_preemption`    | Thread sans yield reçoit quand même le CPU     |

---

## Livrable 3 — Analyse de performance

Écris un benchmark qui compare :
1. Ta bibliothèque `mthread` vs `pthread` (modèle 1:1) pour :
   - Temps de création de N threads
   - Temps de context switch (yield en boucle)
   - Throughput producteur/consommateur

Produis un rapport `benchmark.md` avec les résultats et une explication des différences.

---

## Livrable 4 — Application démo

Implémente **une** des applications suivantes avec ta bibliothèque :

### Option A — Serveur HTTP mono-thread avec coroutines
Un serveur HTTP/1.0 qui gère plusieurs connexions simultanées avec des threads mthread. Chaque connexion = un thread mthread. Teste avec `curl` et `ab` (Apache Benchmark).

### Option B — Calcul parallèle : tri fusion
Implémente un merge sort parallèle. Divise le tableau en N segments, chaque segment trié par un thread mthread. Benchmark vs tri séquentiel.

### Option C — Simulateur de systèmes de files d'attente
Simule M/M/1 (arrivées Poisson, service exponentiel) avec des threads producteurs/consommateurs. Calcule le temps de réponse moyen et compare à la formule théorique.

---

## Critères d'évaluation

| Critère                         | Points |
|---------------------------------|--------|
| Fonctionnalité (API complète)   | 40     |
| Robustesse (tests passent)      | 25     |
| Code propre (lisible, documenté) | 15    |
| Benchmark et analyse            | 10     |
| Application démo                | 10     |
| **Total**                       | **100**|

---

## Conseils de réalisation

### 1. Construis de façon incrémentale
```
Semaine 1 : thread + scheduler FIFO (reprendre TP03)
Semaine 2 : mutex + sémaphore + cond (reprendre TP04-06)
Semaine 3 : préemption + tests stress + benchmark + démo
```

### 2. Tests en premier (TDD)
Écris chaque test avant d'implémenter la fonctionnalité. Un test qui échoue = un objectif clair.

### 3. Valgrind à chaque étape
```bash
valgrind --leak-check=full --track-origins=yes ./test_xxx
```
Ne passe pas à l'étape suivante avec des leaks.

### 4. Git — un commit par fonctionnalité
```bash
git init
git commit -m "feat: mthread_create + scheduler FIFO"
git commit -m "feat: mthread_mutex_lock/unlock"
...
```

### 5. Macros de debug
```c
#ifdef MTHREAD_DEBUG
#define DBG(fmt, ...) fprintf(stderr, "[mthread] " fmt "\n", ##__VA_ARGS__)
#else
#define DBG(fmt, ...)
#endif
// Compile avec -DMTHREAD_DEBUG pour activer
```

---

## Exemple d'utilisation finale

```c
#include "mthread.h"
#include <stdio.h>

void *worker(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 5; i++) {
        printf("Thread %d : étape %d\n", id, i);
        mthread_yield();
    }
    return NULL;
}

int main(void) {
    mthread_set_scheduler(MTHREAD_SCHED_RR);
    mthread_set_quantum(10);   /* 10ms par quantum */

    mthread_t threads[8];
    int ids[8];

    for (int i = 0; i < 8; i++) {
        ids[i] = i;
        mthread_create(&threads[i], worker, &ids[i]);
    }

    for (int i = 0; i < 8; i++)
        mthread_join(threads[i], NULL);

    printf("Tous terminés.\n");
    return 0;
}
```

---

**Bon courage. Ce projet est ambitieux — c'est voulu.**
**À la fin, tu comprendras pourquoi `pthread_mutex_lock` "bloque" et comment Linux choisit quel thread exécuter.**
