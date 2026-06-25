# Cours 07 — Scheduling (Ordonnancement)

---

## 1. Le problème du scheduling

Avec N threads READY et M < N cœurs CPU, qui s'exécute ?

### Définition 1.1 — Scheduler (Ordonnanceur)
Le **scheduler** est le composant qui décide quel thread READY obtient le CPU. Il répond à deux questions :
1. **Quel thread** choisir ?
2. **Quand** reprendre le contrôle (préemption) ?

---

## 2. Scheduling coopératif vs préemptif

### Définition 2.1 — Scheduling coopératif
Le CPU n'est rendu que si le thread en cours le cède **volontairement** (`yield`, `join`, `cond_wait`, `sem_wait`). Le scheduler ne peut pas interrompre un thread qui calcule.

```
Cœur : [T1 calcule 10s][T1 yield][T2 calcule][T2 yield][T1...]
```

- Avantage : simple à implémenter, pas de races sur les structures du scheduler
- Inconvénient : un thread qui oublie `yield` monopolise le CPU (starvation)

### Définition 2.2 — Scheduling préemptif
Le scheduler peut **forcer** un thread à rendre le CPU après un quantum de temps. Sur Unix, on utilise le signal `SIGALRM`.

```
Cœur : [T1===|préempt][T2===|préempt][T1===|préempt]...
           ↑ timer    ↑ timer
```

- Avantage : aucun thread ne peut bloquer le système
- Inconvénient : le timer interrompt à n'importe quel moment → races sur les structures du scheduler → nécessite de masquer les signaux pendant les opérations critiques

---

## 3. Algorithmes d'ordonnancement

### Définition 3.1 — FIFO (First In, First Out)
Le premier thread arrivé dans la run queue est le premier servi. S'exécute jusqu'à ce qu'il se bloque ou termine.

```
Run queue : [T1] → [T2] → [T3]
Exécution : T1 jusqu'à blocage → T2 jusqu'à blocage → ...
```

- Simple, mais risque de starvation si T1 ne se bloque jamais.

### Définition 3.2 — Round-Robin (RR)
Chaque thread reçoit un **quantum** (tranche de temps, ex: 10ms). Quand le quantum expire, le thread est replacé en fin de file.

```
Quantum = 2 unités :
Run queue initial : [T1, T2, T3]
t=0: T1 exécute 2u → fin de file : [T2, T3, T1]
t=2: T2 exécute 2u → fin de file : [T3, T1, T2]
t=4: T3 exécute 1u (termine)   : [T1, T2]
t=5: T1 exécute 2u → ...
```

- Équitable (aucun thread ne peut monopoliser)
- Temps de réponse prévisible

### Théorème 3.1 — Choix du quantum
- Quantum **trop petit** → trop de context switches, overhead important
- Quantum **trop grand** → se rapproche de FIFO, mauvais temps de réponse
- En pratique : 1ms–100ms (Linux utilise ~4ms)

### Définition 3.3 — Scheduling par priorité
Chaque thread a une **priorité**. Le scheduler choisit toujours le thread de plus haute priorité dans la run queue.

```
T1(priorité=3), T2(priorité=1), T3(priorité=2)
→ T1 s'exécute en premier, puis T3, puis T2
```

**Problème — Priority inversion** :
```
T_haute (priorité 10) attend un mutex tenu par T_basse (priorité 1)
T_moyen (priorité 5) s'exécute, bloquant T_basse indirectement
→ T_haute est bloqué plus longtemps que prévu
```
Solution : **priority inheritance** (T_basse hérite temporairement la priorité de T_haute).

### Définition 3.4 — MLFQ (Multi-Level Feedback Queue)
Plusieurs files de priorité. Les threads CPU-intensifs descendent en priorité, les threads interactifs montent. Adaptatif automatiquement.

---

## 4. Métriques d'évaluation

### Définition 4.1 — Turnaround time
Temps entre la création du thread et sa terminaison.
```
turnaround(T) = temps_fin(T) - temps_arrivée(T)
```

### Définition 4.2 — Temps de réponse
Temps entre la création et la première exécution.
```
réponse(T) = temps_première_exécution(T) - temps_arrivée(T)
```

### Définition 4.3 — Starvation
Un thread **ne s'exécute jamais** parce que d'autres threads lui sont toujours préférés. Se produit avec le scheduling par priorité si des threads haute priorité arrivent continuellement.

### Théorème 4.1 — Round-Robin est sans starvation
Dans un scheduler Round-Robin, chaque thread READY s'exécute après au plus `(N-1) × quantum` temps d'attente, où N est le nombre de threads READY.

---

## 5. Implémenter la préemption avec SIGALRM

```c
#include <signal.h>
#include <sys/time.h>

void handler_alarm(int sig) {
    // le signal interrompt le thread courant
    // on fait comme un yield forcé
    mthread_yield();
}

void scheduler_init_preemption(int quantum_ms) {
    // installer le gestionnaire de signal
    struct sigaction sa = {0};
    sa.sa_handler = handler_alarm;
    sigaction(SIGALRM, &sa, NULL);

    // configurer le timer pour envoyer SIGALRM toutes les quantum_ms ms
    struct itimerval timer = {0};
    timer.it_value.tv_usec    = quantum_ms * 1000;  // premier déclenchement
    timer.it_interval.tv_usec = quantum_ms * 1000;  // répétition
    setitimer(ITIMER_REAL, &timer, NULL);
}
```

### Problème — Masquer les signaux pendant les sections critiques

Si `SIGALRM` arrive pendant un `mutex_lock`, le scheduler peut changer de thread pendant que la structure du mutex est modifiée → corruption.

```c
void mutex_lock(mthread_mutex_t *m) {
    sigset_t mask, old_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);   // ← masquer SIGALRM

    // ... modifier la structure du mutex en sécurité ...

    sigprocmask(SIG_SETMASK, &old_mask, NULL);  // ← restaurer le masque
}
```

---

## 6. Structure du scheduler

```c
typedef struct {
    mthread_t  *run_queue_head;   // file des threads READY
    mthread_t  *run_queue_tail;
    mthread_t  *current;          // thread en cours d'exécution
    ucontext_t  ctx;              // contexte du scheduler lui-même
    int         next_id;          // compteur d'IDs
} scheduler_t;

scheduler_t g_scheduler;
```

### La boucle principale du scheduler
```c
void scheduler_run(void) {
    while (1) {
        mthread_t *next = dequeue(&g_scheduler.run_queue_head);

        if (next == NULL) {
            // plus aucun thread READY → tout est terminé ou bloqué
            break;
        }

        next->state = RUNNING;
        g_scheduler.current = next;
        swapcontext(&g_scheduler.ctx, &next->ctx);
        // on revient ici après un yield/blocage/terminaison
    }
}
```

---

## 7. Schéma complet d'un yield

```
Thread T1 (RUNNING) appelle mthread_yield() :

mthread_yield() :
  1. T1.state = READY
  2. enqueue(run_queue, T1)
  3. swapcontext(&T1.ctx, &scheduler.ctx)
     └─→ scheduler reprend dans scheduler_run()
         └─→ next = dequeue(run_queue)   // ex: T2
             └─→ T2.state = RUNNING
                 swapcontext(&scheduler.ctx, &T2.ctx)
                 └─→ T2 reprend là où il était
```

---

## Résumé du cours 07

| Concept            | À retenir                                              |
|--------------------|--------------------------------------------------------|
| Scheduling coopératif | Thread cède volontairement — simple mais risqué   |
| Scheduling préemptif  | Timer force la cession — équitable mais complexe  |
| FIFO               | Premier arrivé, premier servi                          |
| Round-Robin        | Quantum par thread — sans starvation                   |
| Priorité           | Risque d'inversion de priorité et de starvation        |
| SIGALRM            | Outil Unix pour implémenter la préemption              |
| Masquer les signaux| Protéger les structures du scheduler                   |

---

**Précédent** : [Cours 06](cours_06_cond.md)

---

## Vue d'ensemble du projet

Tu as maintenant tous les concepts pour implémenter `mthread`. Voici l'ordre de construction :

```
ucontext_t               (cours 02)
    ↓
mthread_create/join/exit (cours 03)
    ↓
scheduler FIFO           (cours 07)
    ↓
mutex                    (cours 04)
    ↓
sémaphore                (cours 05)
    ↓
variable de condition    (cours 06)
    ↓
scheduler Round-Robin    (cours 07)
```

**Suivant** : [Projet Final](../projet/README.md)
