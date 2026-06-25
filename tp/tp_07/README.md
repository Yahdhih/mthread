# TP 07 — Scheduling Round-Robin (Préemptif)

> Prérequis : TP 03 à 06 fonctionnels.
> Objectif : passer d'un scheduler FIFO coopératif à Round-Robin préemptif via SIGALRM.
> Durée estimée : 4h (le plus difficile des TP).

---

## Ce que tu vas implémenter

Modifier le scheduler de TP 03 pour :
1. Passer de FIFO à **Round-Robin** (même algorithme, mais avec préemption)
2. Utiliser `SIGALRM` + `setitimer` pour forcer un yield toutes les `QUANTUM_MS`
3. Masquer `SIGALRM` pendant les sections critiques du scheduler

---

## Plan d'implémentation

### Étape 1 — SIGALRM handler

```c
#include <signal.h>
#include <sys/time.h>

static void sigalrm_handler(int sig) {
    (void)sig;
    mthread_yield();    /* yield forcé */
}

void scheduler_enable_preemption(int quantum_ms) {
    struct sigaction sa = {0};
    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);

    struct itimerval tv = {0};
    tv.it_value.tv_usec    = quantum_ms * 1000;
    tv.it_interval.tv_usec = quantum_ms * 1000;
    setitimer(ITIMER_REAL, &tv, NULL);
}
```

### Étape 2 — Masquage de SIGALRM

```c
void scheduler_block_signal(void) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void scheduler_unblock_signal(void) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}
```

Appelle `block_signal` au début de `mutex_lock`, `cond_wait`, etc. et `unblock_signal` à la fin.

### Étape 3 — Test

Compile un test où T1 fait un calcul infini (pas de yield). Avec préemption, T2 devrait quand même s'exécuter.

---

## Tests à valider

### Test 1 — Thread sans yield forcé à partager le CPU
```c
void *calcul_infini(void *arg) {
    while(1) /* pas de yield */ ;
}
// T2 devrait afficher des messages malgré T1
```

### Test 2 — Équité Round-Robin
5 threads de même durée : vérifier que chacun reçoit ~20% du CPU.

### Test 3 — Préemption n'interfère pas avec les mutex
Un mutex doit rester cohérent même si le signal arrive pendant lock/unlock.

---

## Mise en garde

La préemption rend le débogage difficile. Utilise :
```bash
gcc -g -fsanitize=thread ./test_preemptif.c mthread.c -o test
# Attention : TSan ne fonctionne pas bien avec SIGALRM sur certains systèmes
```

Si tu vois des comportements aléatoires, c'est probablement un oubli de masquage.
