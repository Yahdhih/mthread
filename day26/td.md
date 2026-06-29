# TD — Exercices 2, 3, 4

---

## Exercice 2 — Inversion de Priorité : Tracer le Scénario

**Scheduler à priorités** : le thread de plus haute priorité en file READY s'exécute toujours en premier.

```
T_bas   (prio=1) : verrouille M, calcule 4u, déverrouille M
T_haut  (prio=3) : verrouille M — bloqué si M pris
T_moyen (prio=2) : calcule 3u (ne touche pas M)
```

**Timeline :**
```
t=0 : T_bas démarre, lock(M)
t=1 : T_haut arrive, tente lock(M) → bloqué (M pris)
t=2 : T_moyen arrive
```

**a)** Sans héritage de priorité — complète la timeline de t=2 à t=fin.
(Rappel : scheduler à priorités → le thread READY de plus haute prio s'exécute)
```
t=2 : File READY = [T_moyen, T_bas]. Qui s'exécute ?
```

**b)** Combien de temps T_haut (prio=3) attend-il ? Qui le retarde indirectement ?

**c)** Avec héritage de priorité — retrace la timeline depuis t=1.
(T_bas hérite la prio=3 de T_haut quand T_haut est bloqué sur M)

**d)** Le gain de temps est-il significatif ? Dans quel scénario l'inversion de priorité est-elle catastrophique ?

---

## Exercice 3 — SIGALRM et Sections Critiques

```c
static void sigalrm_handler(int sig) {
    (void)sig;
    mthread_yield();
}

void mthread_yield(void) {
    current->state = MTHREAD_READY;
    enqueue(current);           /* (A) */
    swapcontext(&current->ctx,  /* (B) */
                &sched_ctx);
}
```

**a)** Que se passe-t-il si `SIGALRM` arrive exactement **entre** les instructions `(A)` et `(B)` ?
Trace le problème :
```
Thread T1 exécute mthread_yield() :
  (A) enqueue(T1) → ready_queue = [T1]
  ← SIGALRM arrive ICI
  handler: mthread_yield() est appelé
    (A) enqueue(T1) à nouveau → ready_queue = [T1, T1] ???
    (B) swapcontext...
  Retour au premier yield : (B) swapcontext...
Résultat :
```

**b)** Même question si SIGALRM arrive pendant `enqueue()` (pendant la modification de `ready_queue->next`).

**c)** Quelle est la solution ? Écris le code correct pour `mthread_yield()` avec masquage.

---

**Espace de réponse :**
```
a)

b)

c) void mthread_yield(void) {
       // masquer SIGALRM
       sigprocmask(...);
       current->state = MTHREAD_READY;
       enqueue(current);
       swapcontext(&current->ctx, &sched_ctx);
       // démasquer SIGALRM
       sigprocmask(...);
   }
```

---

## Exercice 4 — Round-Robin : Calcul de Temps de Réponse

**5 processus CPU-bound** (tous arrivent à t=0) :

| Processus | Durée |
|-----------|-------|
| P1        | 10    |
| P2        | 3     |
| P3        | 7     |
| P4        | 2     |
| P5        | 5     |

**a)** FIFO (ordre d'arrivée P1, P2, P3, P4, P5) :
- Calcule le temps de complétion de chaque processus
- Calcule le temps de réponse moyen (temps entre t=0 et première exécution)
- Calcule le temps d'attente moyen

**b)** Round-Robin avec quantum=3 :
- Trace le diagramme de Gantt
- Recalcule les mêmes métriques

**c)** Quelle politique est meilleure pour l'**interactivité** (latence minimale) ? Pour le **débit** (throughput) ?

---

## Corrigés

<details>
<summary>Exercice 2</summary>

a) Timeline sans héritage :
```
t=0: T_bas lock(M)
t=1: T_haut bloqué (M pris). File READY=[T_bas]
t=2: T_moyen arrive. File READY=[T_moyen, T_bas]
     Scheduler: T_moyen prio=2 > T_bas prio=1 → T_moyen s'exécute
t=3: T_moyen continue
t=4: T_moyen continue
t=5: T_moyen finit. File READY=[T_bas]
     T_bas s'exécute
t=6: T_bas calcule
t=7: T_bas calcule
t=8: T_bas calcule
t=9: T_bas unlock(M) → T_haut débloqué, enqueued prio=3
     Scheduler: T_haut s'exécute
t=9: T_haut lock(M), s'exécute
```

b) T_haut attend de t=1 à t=9 = 8 unités. T_moyen (prio=2) le retarde pendant 3 unités alors qu'il n'a rien à voir avec M.

c) Avec héritage :
```
t=1: T_haut bloqué → T_bas.prio hérite 3
t=2: T_moyen arrive. File READY=[T_bas(prio=3), T_moyen(prio=2)]
     T_bas continue (prio=3 > 2)
t=3-5: T_bas finit son calcul (sans interruption)
t=5: T_bas unlock(M), prio revient à 1, T_haut débloqué
t=5: T_haut lock(M), s'exécute (prio=3)
```
T_haut attend seulement t=1 à t=5 = 4 unités.

d) Ici gain de 4 unités. Catastrophique dans les systèmes temps-réel : une tâche critique (T_haut) pourrait manquer une deadline (ex : contrôle de moteur, Mars Pathfinder).

</details>

<details>
<summary>Exercice 3</summary>

a) T1 est ajouté 2 fois dans la file. Quand le scheduler parcourt la file, il exécute T1 deux fois (ou la liste chaînée est corrompue via `next`). Comportement indéfini.

b) Si SIGALRM arrive pendant `enqueue()` pendant la modification de `p->next = T1`, la liste est dans un état incohérent. Le handler appelle `enqueue` à nouveau → corruption de pointeur → segfault probable.

c)
```c
void mthread_yield(void) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);   // masquer
    current->state = MTHREAD_READY;
    enqueue(current);
    swapcontext(&current->ctx, &sched_ctx);
    sigprocmask(SIG_UNBLOCK, &set, NULL); // démasquer
}
```

</details>

<details>
<summary>Exercice 4</summary>

a) FIFO : P1[0-10], P2[10-13], P3[13-20], P4[20-22], P5[22-27]
- Temps de réponse (1ère exec) : P1=0, P2=10, P3=13, P4=20, P5=22 → moyen=13
- Temps de complétion : 10,13,20,22,27 → moyen=18.4

b) RR (quantum=3) :
t=0-3: P1 (reste 7)
t=3-6: P2 (reste 0, fini à t=6)
t=6-9: P3 (reste 4)
t=9-12: P4 → P4 finit à t=11, P5 commence à t=11
t=11-14: P5 (reste 2)
t=14-17: P1 (reste 4)
t=17-20: P3 (reste 1)
t=20-21: P5 (reste 0, fini t=21)
...
- Temps de réponse moyen : P1=0, P2=3, P3=6, P4=9, P5=11 → moyen=5.8
- BIEN meilleur pour l'interactivité que FIFO (13)

c) Round-Robin est meilleur pour l'**interactivité** (latence/temps de réponse). FIFO peut être meilleur pour le **débit** (moins de context switches = moins d'overhead).

</details>
