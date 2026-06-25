# TD 07 — Scheduling

> Exercices sur papier. Durée estimée : 2h.

---

## Exercice 1 — Simulation FIFO et Round-Robin

**Données :**

| Thread | Arrivée | Durée CPU |
|--------|---------|-----------|
| T1     | t=0     | 8 unités  |
| T2     | t=2     | 4 unités  |
| T3     | t=4     | 2 unités  |

**Partie A — FIFO (non préemptif)**

Dessine le diagramme de Gantt et calcule :
- Temps de terminaison de chaque thread
- Turnaround time de chaque thread
- Turnaround moyen
- Temps de réponse de chaque thread

**Partie B — Round-Robin, quantum = 2**

Même calculs. Note les préemptions.

**Partie C — Comparaison**

Quel algorithme est meilleur pour le turnaround moyen ? Pour le temps de réponse moyen ?

---

## Exercice 2 — Scheduling coopératif : analyse

Considère ce programme avec scheduling coopératif :

```c
void *T1(void *arg) {
    while (1) {
        calculer();       // 100ms de CPU
        // PAS de yield ici
    }
}

void *T2(void *arg) {
    while (1) {
        yield();
        afficher();       // 1ms
    }
}
```

a) Avec un scheduler coopératif, T2 s'exécute-t-il ? Pourquoi ?

b) Si T1 ajoute `yield()` après `calculer()`, quelle est la répartition du CPU entre T1 et T2 ?

c) Dans un scheduler préemptif avec quantum=10ms, quelle est la répartition du CPU ?

d) Quel scheduler est préférable pour un système interactif (UI) ? Pourquoi ?

---

## Exercice 3 — Priority Inversion

**Scénario :**
- T_haute : priorité 10, attend le mutex M
- T_moyen : priorité 5, pas de mutex
- T_basse  : priorité 1, détient le mutex M

Trace l'exécution avec un scheduler par priorité (préemptif) :

```
t=0 : T_basse démarre, acquiert M
t=1 : T_haute démarre (priorité 10 > 1 → préempte T_basse)
t=2 : T_haute tente d'acquérir M → BLOQUÉ
t=2 : T_moyen démarre
...
```

a) Combien de temps T_haute attend-elle avant d'obtenir M ? T_moyen s'exécute-t-il avant T_haute ?

b) C'est le problème de **priority inversion**. Explique en quoi c'est paradoxal.

c) La solution est le **priority inheritance** : T_basse hérite temporairement la priorité de T_haute. Retrace l'exécution avec cette solution.

*(Note historique : ce bug a causé des resets du robot Mars Pathfinder en 1997)*

---

## Exercice 4 — Starvation

**Scénario :**
- Scheduler par priorité (préemptif)
- T_travail : priorité 1, calcul long
- T_interactif_1..10 : priorité 10, arrivent aléatoirement, courtes durées

a) T_travail s'exécute-t-il jamais si T_interactif_i arrivent continuellement ?

b) Le scheduler Linux utilise le **aging** : la priorité d'un thread augmente avec le temps passé en attente. Comment cela résout-il la starvation ?

c) Avec aging, est-il possible qu'un thread interactif ait une moins bonne expérience ? Quel est le compromis ?

---

## Exercice 5 — Implémenter yield avec swapcontext

Voici le pseudo-code de `mthread_yield()` :

```
yield() :
  1. ???
  2. ???
  3. swapcontext(&current->ctx, &scheduler.ctx)
```

a) Complète les étapes 1 et 2.

b) Que se passe-t-il dans le scheduler après que `swapcontext` rende la main ?

c) Quelle est la différence entre `mthread_yield()` et `mthread_exit()` dans leur implémentation ?

d) Dans un scheduler préemptif, le handler `SIGALRM` appelle `mthread_yield()`. Pourquoi faut-il masquer `SIGALRM` dans `mthread_yield()` lui-même ?

---

## Exercice 6 — Choix du quantum

Pour chaque application, recommande un quantum et justifie :

| Application                              | Quantum recommandé | Justification |
|------------------------------------------|--------------------|---------------|
| Jeu vidéo 60fps (besoin de 16ms/frame)   | ?                  | ?             |
| Calcul scientifique (matrices 1000x1000) | ?                  | ?             |
| Serveur web (requêtes courtes <1ms)      | ?                  | ?             |
| Éditeur de texte (interactif)            | ?                  | ?             |

---

## Corrigé

<details>
<summary>Exercice 1 — FIFO</summary>

```
Diagramme de Gantt FIFO :
[T1: 0-8][T2: 8-12][T3: 12-14]

Terminaison : T1=8, T2=12, T3=14
Turnaround  : T1=8-0=8, T2=12-2=10, T3=14-4=10
Moyen       : (8+10+10)/3 = 9.33

Réponse     : T1=0, T2=8, T3=12
```

</details>

<details>
<summary>Exercice 1 — Round-Robin quantum=2</summary>

```
t=0 : T1 (2u) → [0-2]
t=2 : T2 arrive, file = [T2, T1]. T2 (2u) → [2-4]
t=4 : T3 arrive, file = [T1, T3]. T1 (2u) → [4-6]
t=6 : file = [T3, T2]. T3 (2u) → [6-8]. T3 termine.
t=8 : file = [T2, T1]. T2 (2u) → [8-10]. T2 termine.
t=10: T1 (2u) → [10-12]. T1 termine.

Terminaison : T1=12, T2=10, T3=8
Turnaround  : T1=12, T2=8, T3=4. Moyen = 8
Réponse     : T1=0, T2=2, T3=4. Moyen = 2
```

Conclusion : RR a un meilleur temps de réponse (surtout pour T3). FIFO peut avoir meilleur turnaround si les threads sont ordonnés par durée croissante.

</details>

<details>
<summary>Exercice 5</summary>

```
yield() :
  1. current->state = READY
  2. enqueue(run_queue, current)
  3. swapcontext(&current->ctx, &scheduler.ctx)

exit() :
  1. current->state = ZOMBIE        ← ne remet PAS dans run_queue
  2. current->retval = valeur
  3. réveiller le thread join() si présent
  4. setcontext(&scheduler.ctx)     ← pas de sauvegarde (ne reviendra pas)
```

</details>
