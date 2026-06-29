# Cours — Scheduling

---

## 1. Scheduling Coopératif vs Préemptif

### Définition 1.1 — Scheduling Coopératif
Dans le scheduling **coopératif**, un thread ne perd le CPU que s'il appelle explicitement `yield()` ou se bloque (mutex, sem, cond).

**Avantages** : simple à implémenter, pas de corruption dans les sections critiques.
**Inconvénients** : un thread "égoïste" (boucle infinie sans yield) bloque tous les autres.

### Définition 1.2 — Scheduling Préemptif
Dans le scheduling **préemptif**, le système peut forcer un thread à rendre le CPU après un **quantum** (tranche de temps).

**Avantages** : équitable, aucun thread ne peut monopoliser le CPU.
**Inconvénients** : plus complexe, nécessite des sections critiques protégées contre les signaux.

---

## 2. FIFO (First In, First Out)

### Définition 2.1 — FIFO
Le premier thread mis en file READY est le premier à s'exécuter. Pas de préemption.

```
File READY au départ : [T1, T2, T3]
T1 s'exécute jusqu'à yield/blocage → [T2, T3, T1] (si T1 yield)
T2 s'exécute → [T3, T1]
...
```

**Risque** : si T1 ne yield jamais, T2 et T3 ne s'exécutent jamais → **famine**.

---

## 3. Round-Robin

### Définition 3.1 — Round-Robin
Chaque thread a droit à un **quantum** de temps (ex : 10 ms). Après ce quantum, le scheduler passe au suivant (comme FIFO mais forcé par un timer).

```
Quantum=2 unités, threads T1(6u), T2(4u), T3(2u) :
t=0  T1 [2u]
t=2  T2 [2u]
t=4  T3 [2u] → T3 terminé
t=6  T1 [2u]
t=8  T2 [2u] → T2 terminé
t=10 T1 [2u] → T1 terminé
```

**Avantage** : aucun thread ne monopolise le CPU.
**Inconvénient** : overhead si quantum trop petit (beaucoup de context switches).

---

## 4. Inversion de Priorité

### Définition 4.1 — Inversion de Priorité
Une tâche de **haute priorité** est bloquée indirectement par une tâche de **basse priorité**, via une tâche de priorité intermédiaire qui préempte la basse priorité.

**Scénario** :
```
T_haut (prio=3) : veut verrou M → bloqué
T_bas  (prio=1) : détient M
T_moyen(prio=2) : préempte T_bas → T_haut attend T_moyen !
```

**Solution** — Héritage de priorité :
Quand T_haut attend un verrou détenu par T_bas, T_bas hérite temporairement de la priorité de T_haut.

---

## 5. Préemption avec SIGALRM

### Définition 5.1 — SIGALRM
`SIGALRM` est un signal envoyé par le noyau après un délai configuré.

```c
#include <signal.h>
#include <sys/time.h>

static void sigalrm_handler(int sig) {
    (void)sig;
    mthread_yield();  /* forcer un yield */
}

void preempt_enable(void) {
    struct sigaction sa;
    sa.sa_handler = sigalrm_handler;
    sa.sa_flags   = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);

    struct itimerval timer;
    timer.it_value.tv_sec     = 0;
    timer.it_value.tv_usec    = 10000;   /* 10 ms */
    timer.it_interval         = timer.it_value;
    setitimer(ITIMER_REAL, &timer, NULL);
}
```

---

## 6. Masquage des Signaux — `sigprocmask`

### Définition 6.1 — Masque de signaux
Le masque de signaux détermine quels signaux sont **bloqués** (différés, pas ignorés).

```c
sigset_t alrm_set;
sigemptyset(&alrm_set);
sigaddset(&alrm_set, SIGALRM);

/* Avant section critique */
sigprocmask(SIG_BLOCK, &alrm_set, NULL);

/* ... section critique (swapcontext, enqueue, dequeue) ... */

/* Après section critique */
sigprocmask(SIG_UNBLOCK, &alrm_set, NULL);
```

### Théorème 6.1 — Pourquoi masquer ?
Si `SIGALRM` arrive pendant `swapcontext()` ou pendant la manipulation de la file READY :
- Le gestionnaire appelle `mthread_yield()` qui appelle encore `swapcontext()`
- La pile du scheduler est corrompue → comportement indéfini

**Règle** : masquer SIGALRM dans toute section critique de la bibliothèque.

---

## 7. Starvation

### Définition 7.1 — Starvation (Famine)
Un thread est en **famine** s'il attend indéfiniment sans jamais être schedulé.

**Causes** :
- FIFO + un thread qui ne yield jamais
- Priorités statiques + threads haute priorité qui arrivent en continu

**Solution — Aging (Vieillissement)** :
Augmenter progressivement la priorité d'un thread qui attend depuis longtemps.

```
Chaque N ms sans être schedulé : prio++
```

---

## Résumé

| Concept         | À retenir                                          |
|-----------------|----------------------------------------------------|
| Coopératif      | Thread cède volontairement le CPU                  |
| Préemptif       | Timer force le yield (SIGALRM + setitimer)         |
| FIFO            | Simple, risque de famine                           |
| Round-Robin     | Équitable, quantum de temps                        |
| Inversion prio  | Haute prio bloquée par intermédiaire — héritage    |
| sigprocmask     | Masquer SIGALRM autour de swapcontext              |
| Starvation      | Aging comme remède                                 |
