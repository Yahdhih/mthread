# TD03 — Exercices 2 à 5 : cycle de vie en profondeur

---

## Exercice 2 — Joindre un thread qui n'existe pas

### Question 2.1

Que se passe-t-il si on appelle `mthread_join` sur un `mthread_t` qui a été initialisé à `NULL` ou sur un pointeur invalide ?

```
[espace de réponse]
```

### Question 2.2

Et si on appelle `mthread_join` sur un thread déjà joint une première fois (son `mthread_struct_t` a déjà été libéré) ?

```
[espace de réponse]
```

### Question 2.3

Dans la bibliothèque pthread (threads POSIX), `pthread_join` sur un thread invalide retourne `ESRCH`. Notre implémentation mthread devrait-elle aussi vérifier la validité ? Quelles vérifications minimales peut-on faire ?

```
[espace de réponse]
```

<details>
<summary>Corrigé exercice 2</summary>

**Q2.1 — join sur NULL :**

Comportement indéfini (UB). Notre implémentation va déréférencer le pointeur `NULL` dès la première ligne de `mthread_join` (pour lire `thread->state`). Résultat : **SIGSEGV** (segmentation fault) et crash immédiat.

**Q2.2 — join sur thread déjà libéré :**

Également UB, mais plus insidieux. La mémoire libérée peut avoir été réallouée pour autre chose. On lit des données corrompues. On peut obtenir un état arbitraire, un crash, ou une exécution silencieusement incorrecte. C'est un **use-after-free** — détectable avec Valgrind ou AddressSanitizer.

**Q2.3 — Vérifications minimales :**

```c
int mthread_join(mthread_t thread, void **retval) {
    if (thread == NULL) return -1;          // vérification minimale
    if (thread->state == ...) { ... }
}
```

Mais il est difficile de détecter "thread déjà libéré" sans bookkeeping supplémentaire (table de tous les threads vivants, compteur de références...). Dans notre implémentation simplifiée, on fait confiance à l'utilisateur pour ne pas joindre deux fois.

</details>

---

## Exercice 3 — Double join circulaire : deadlock ?

Considère ce scénario :

```c
void *func_A(void *arg) {
    mthread_t *B = (mthread_t *)arg;
    mthread_join(*B, NULL);   // A attend B
    return NULL;
}

void *func_B(void *arg) {
    mthread_t *A = (mthread_t *)arg;
    mthread_join(*A, NULL);   // B attend A
    return NULL;
}

int main(void) {
    mthread_t tA, tB;
    mthread_create(&tA, func_A, &tB);
    mthread_create(&tB, func_B, &tA);
    mthread_yield();
    return 0;
}
```

### Question 3.1

Que se passe-t-il au moment où A appelle `mthread_join(B)` ?

```
[espace de réponse]
```

### Question 3.2

Que se passe-t-il au moment où B appelle `mthread_join(A)` ?

```
[espace de réponse]
```

### Question 3.3

Qui va réveiller A ? Qui va réveiller B ? Y a-t-il un deadlock ?

```
[espace de réponse]
```

### Question 3.4

Quelles sont les 4 conditions de Coffman (nécessaires au deadlock) ? Laquelle s'applique ici ?

```
[espace de réponse]
```

<details>
<summary>Corrigé exercice 3</summary>

**Q3.1 — A appelle join(B) :**

B n'est pas encore ZOMBIE (il n'a pas encore eu le CPU pour s'exécuter, ou il est en READY). Donc A prend le chemin "cas 2" :
- `A->state = BLOCKED`
- `B->joiner = A`
- `swapcontext(A, scheduler)`

A s'endort. La file READY contient [B] (ou [main, B] selon l'ordre).

**Q3.2 — B appelle join(A) :**

A est en état BLOCKED. B prend le chemin "cas 2" :
- `B->state = BLOCKED`
- `A->joiner = B`
- `swapcontext(B, scheduler)`

B s'endort. La file READY est **vide** (main a déjà yielded et n'a personne à réveiller).

**Q3.3 — Qui réveille qui :**

- A sera réveillé quand B appellera `mthread_exit`. Mais B est BLOCKED.
- B sera réveillé quand A appellera `mthread_exit`. Mais A est BLOCKED.
- Le scheduler trouve la file vide et retourne au main. Main sort du yield (si la file était vide quand main a yielded, il retourne immédiatement).
- Si main retourne de yield et fait `return 0`, le programme se termine avec A et B toujours bloqués — leurs mémoires sont libérées par le système. Ce n'est pas un vrai deadlock infini dans ce cas, mais c'est une corruption logique.

En réalité, si le programme ne sort pas du main, c'est un **deadlock** : deux threads se bloquent mutuellement dans un cycle.

**Q3.4 — Conditions de Coffman :**

1. **Exclusion mutuelle** : une ressource ne peut être détenue que par un seul thread.
2. **Hold and wait** : un thread détient une ressource et attend une autre.
3. **Non-préemption** : une ressource ne peut être reprise de force.
4. **Attente circulaire** : il existe un cycle dans le graphe d'attente.

Ici : A attend B, B attend A → **attente circulaire**. Les 4 conditions sont réunies (la "ressource" est le thread-cible du join).

</details>

---

## Exercice 4 — Visualiser la file READY

Considère ce scénario avec 4 threads créés dans l'ordre T1, T2, T3, T4. Le main crée les threads puis yield.

**Séquence d'événements :**
1. Main crée T1 (READY)
2. Main crée T2 (READY)
3. Main crée T3 (READY)
4. Main crée T4 (READY)
5. Main yield
6. T1 s'exécute, yield
7. T2 s'exécute, exit
8. T3 s'exécute, yield
9. T4 s'exécute, exit
10. T1 s'exécute, exit
11. T3 s'exécute, exit
12. Main reprend (file vide)

### Question 4.1

Dessine l'état de la file READY **après chaque événement** numéroté ci-dessus.

```
Après (1) : [T1]
Après (2) : [T1, T2]
Après (3) : ?
Après (4) : ?
Après (5) : ? [main ajoute main en queue, scheduler défile T1]
Après (6) : ?
Après (7) : ?
Après (8) : ?
Après (9) : ?
Après (10): ?
Après (11): ?
Après (12): vide
```

```
[complète ici]
```

### Question 4.2

À quel moment main reprend-il après son yield initial (événement 5) ?

```
[espace de réponse]
```

<details>
<summary>Corrigé exercice 4</summary>

**Q4.1 — État de la file :**

```
Après (1)  create T1  : [T1]
Après (2)  create T2  : [T1, T2]
Après (3)  create T3  : [T1, T2, T3]
Après (4)  create T4  : [T1, T2, T3, T4]
Après (5)  main yield : main→READY, enqueue(main), file=[T1, T2, T3, T4, main]
                        scheduler défile T1, T1→RUNNING
                        file après défile : [T2, T3, T4, main]
Après (6)  T1 yield   : T1→READY, enqueue(T1), file=[T2, T3, T4, main, T1]
                        scheduler défile T2, T2→RUNNING
                        file : [T3, T4, main, T1]
Après (7)  T2 exit    : T2→ZOMBIE, pas dans la file
                        scheduler défile T3, T3→RUNNING
                        file : [T4, main, T1]
Après (8)  T3 yield   : T3→READY, enqueue(T3), file=[T4, main, T1, T3]
                        scheduler défile T4, T4→RUNNING
                        file : [main, T1, T3]
Après (9)  T4 exit    : T4→ZOMBIE
                        scheduler défile main, main→RUNNING
                        file : [T1, T3]
Après (10) main reprend (étape 12 dans la séquence — voir note)
           Mais main fait mthread_join...
           
Note : en fait "main reprend" signifie que le scheduler a défilé main.
       Si main fait ensuite des joins, c'est la séquence attendue.

Après (10) T1 exit    : T1→ZOMBIE, file=[T3] (T1 sort de la file car exit)
Après (11) T3 exit    : T3→ZOMBIE, file=vide
Après (12) main reprend (scheduler trouve file vide, return vers main)
```

**Q4.2 — Quand main reprend :**

Main reprend après événement (9) quand T4 exit. À ce moment, le scheduler défile main (qui est en tête de la file [main, T1, T3]). Main est le prochain à s'exécuter.

</details>

---

## Exercice 5 — malloc en modèle M:1 coopératif

Dans notre modèle mthread (M threads utilisateur sur 1 thread noyau, coopératif), plusieurs threads peuvent appeler `malloc()`.

### Question 5.1

Dans le modèle M:1 coopératif, y a-t-il un risque de race condition lors d'un appel à `malloc()` ?

```
[espace de réponse]
```

### Question 5.2

Dans un modèle M:N (plusieurs threads noyau), est-ce qu'un appel à `malloc()` est thread-safe sur Linux ?

```
[espace de réponse]
```

### Question 5.3

Dans notre modèle M:1, si un thread appelle `malloc()` et que `malloc()` prend très longtemps (fragmentation du tas, appel à `brk()` ou `mmap()`), qu'est-ce que ça implique pour les autres threads ?

```
[espace de réponse]
```

### Question 5.4 (bonus)

Dans un vrai système de threads (M:N), les implémentations de `malloc` utilisent des **arènes** (pools de mémoire par thread). Pourquoi ? Quel problème résout cette technique ?

```
[espace de réponse]
```

<details>
<summary>Corrigé exercice 5</summary>

**Q5.1 — malloc en M:1 coopératif :**

**Non**, il n'y a pas de race condition. Dans le modèle coopératif M:1, un seul thread s'exécute à la fois. Un thread ne peut être interrompu que s'il appelle explicitement `yield`, `join`, ou `exit`. `malloc()` ne fait aucun de ces appels — il s'exécute donc de façon **atomique du point de vue des autres threads**. Quand `malloc` retourne, l'appelant a son pointeur, personne d'autre n'a touché au tas pendant ce temps.

**Q5.2 — malloc thread-safe en M:N :**

Oui, la glibc (et les autres implémentations modernes comme jemalloc, tcmalloc) est **thread-safe**. Elle utilise des mutex internes pour protéger les structures du tas contre les accès concurrents. Donc `malloc` peut être appelé depuis plusieurs threads simultanément sans corruption.

**Q5.3 — malloc lent en M:1 :**

C'est le problème fondamental du modèle M:1. Si un thread bloque sur `malloc()` pendant un appel système lent (`brk()`, `mmap()`), **tous les autres threads utilisateur sont bloqués aussi** — car ils s'exécutent tous sur le même thread noyau. Ce thread noyau est bloqué en attente du retour du syscall.

C'est l'une des limitations majeures du modèle M:1 : un seul appel système bloquant paralyse tout le système de threads. C'est pour ça que les vrais systèmes utilisent M:N.

**Q5.4 — Arènes (bonus) :**

Les arènes sont des pools de mémoire indépendants, un par thread (ou par groupe de threads). Chaque thread alloue depuis sa propre arène → **pas de contention** sur un mutex global. Avantages : meilleure performance (pas de lock contention), meilleure localité cache. `glibc ptmalloc2` utilise ce mécanisme avec jusqu'à 8 × nombre de cœurs arènes.

</details>

---

➡️ Demain : `day10/` — TP03 : mthread_create et scheduler (TODO 1-7)
