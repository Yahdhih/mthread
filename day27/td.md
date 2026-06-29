# TD — Exercices 5, 6 et Révision

---

## Exercice 5 — Scheduler à Priorités

Conçois un scheduler à priorités (1=basse, 10=haute).

**a)** Quelle structure de données utiliser pour la file des threads READY ?
Compare : tableau de N files (une par niveau de priorité) vs file unique triée.

**b)** Implémente `schedule_next()` en pseudocode qui retourne toujours le thread de plus haute priorité :

```
// Option A : tableau de files (N=10 niveaux)
mthread_struct_t *ready[N_PRIO];  // ready[i] = file de priorité i

mthread_struct_t *schedule_next(void) {
    // parcourir de N_PRIO-1 à 0
    // retourner le premier thread trouvé
}

// Option B : file unique, triée par priorité
// enqueue trie par priorité, dequeue retire la tête
```

**c)** Avec le scheduler à priorités, définis l'algorithme d'héritage de priorité :

```
mutex_lock(m):
    si m->locked :
        t_owner = qui détient m ?
        si current->prio > t_owner->prio :
            t_owner->prio = current->prio  // héritage
        // bloquer current...

mutex_unlock(m):
    // restaurer prio originale de current
    current->prio = current->original_prio
    // réveiller un waiter...
```

**d)** Problème : si T1 (prio=5) détient M1, T2 (prio=7) détient M2 et attend M1, T3 (prio=9) attend M2 — comment propager l'héritage en chaîne ?

---

## Exercice 6 — Starvation et Vieillissement (Aging)

**a)** Donne un scénario précis où un thread T_bas (prio=1) est en famine dans un scheduler à priorités.

**b)** Algorithme d'aging :

```
Toutes les K ms (via SIGALRM ou timer dédié) :
    pour chaque thread t dans la file READY :
        t->wait_time += K
        si t->wait_time > AGING_THRESHOLD :
            t->prio = min(t->prio + 1, MAX_PRIO)
            t->wait_time = 0
```

Pour K=10ms et AGING_THRESHOLD=100ms :
- Combien de temps T_bas (prio=1) attend-il avant d'atteindre prio=5 ?
- Prio=5 suffit-elle à s'exécuter si des threads prio=6 arrivent en continu ?

**c)** Quelle valeur de AGING_THRESHOLD est raisonnable ? Quels compromis fait-on en la rendant plus grande ou plus petite ?

---

## Tableau de Révision Générale

Remplis ce tableau (tes réponses, pas de corrigé imposé — c'est ta compréhension qui compte) :

| Primitive      | Cas d'usage idéal | Ne pas utiliser quand | Risque principal |
|----------------|-------------------|-----------------------|-----------------|
| `mutex`        |                   |                       |                 |
| `sem` (compteur)|                  |                       |                 |
| `sem` (binaire)|                   |                       |                 |
| `cond_wait`    |                   |                       |                 |
| `cond_broadcast`|                  |                       |                 |
| `yield` (coopératif)|              |                       |                 |
| `SIGALRM` (préemptif)|             |                       |                 |

---

## Corrigés

<details>
<summary>Exercice 5</summary>

a) Tableau de N files : O(1) pour enqueue et dequeue (chercher la file non vide depuis le haut). File unique triée : O(n) pour enqueue (insertion triée), O(1) pour dequeue. Le tableau est plus efficace pour un nombre fixe de niveaux.

b) schedule_next avec tableau :
```c
mthread_struct_t *schedule_next(void) {
    for (int p = MAX_PRIO - 1; p >= 0; p--) {
        if (ready[p]) {
            mthread_struct_t *t = ready[p];
            ready[p] = t->next;
            t->next = NULL;
            return t;
        }
    }
    return NULL;  // rien à faire
}
```

c) Voir le pseudocode. La clé : sauvegarder `original_prio` et la restaurer au unlock.

d) Propagation en chaîne (priority inheritance transitif) :
```
mutex_lock(M1) par T2 (attend M1 détenu par T1) :
    T1.prio = max(T1.prio, T2.prio) = 7
    T2 détient M2, T3 attend M2 :
        T2.prio = max(T2.prio, T3.prio) = 9
        → propager : T1.prio = max(T1.prio, T2.prio_effectif=9) = 9
```

</details>

<details>
<summary>Exercice 6</summary>

a) Scénario : T_haut (prio=9) tourne en boucle, se remet en file READY via yield, est toujours choisi avant T_bas (prio=1). T_bas peut attendre indéfiniment.

b) T_bas (prio=1) → après 100ms : prio=2 → après 200ms : prio=3 → … → après 400ms : prio=5. Soit 400ms pour atteindre prio=5. Si threads prio=6 arrivent en continu, T_bas est encore en famine. Il faudrait qu'il monte à prio=7 pour passer avant eux.

c) AGING_THRESHOLD grand : peu de promotions, équité lente à s'établir, moins d'overhead. AGING_THRESHOLD petit : promotions fréquentes, risque d'inverser les priorités trop vite, overhead élevé. Valeur typique : 100ms-500ms selon les exigences de latence.

</details>
