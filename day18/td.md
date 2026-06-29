# TD — Exercices 2, 3, 4, 5

---

## Exercice 2 — Lecteurs / Écrivains

**Problème** : plusieurs threads lisent et écrivent une ressource partagée.
- Plusieurs lecteurs peuvent lire simultanément.
- Un écrivain doit avoir un accès exclusif (ni lecteur ni autre écrivain).

**a)** Peut-on résoudre ce problème avec un seul sémaphore binaire ? Quel problème pose cette approche ?

**b)** Solution avec : `sem_t write_lock` (init=1) + `int read_count` (init=0) + `sem_t count_lock` (init=1).
Complète le pseudocode :

```
Lecteur:
    sem_wait(count_lock)
    read_count++
    si read_count == 1 : sem_wait(write_lock)  // premier lecteur bloque les écrivains
    sem_post(count_lock)
    // --- lire ---
    sem_wait(count_lock)
    read_count--
    si read_count == 0 : ___________________   // dernier lecteur libère
    sem_post(count_lock)

Écrivain:
    sem_wait(write_lock)
    // --- écrire ---
    ___________________
```

**c)** Cette solution favorise-t-elle les lecteurs ou les écrivains ? Quel problème peut surgir pour les écrivains ?

---

## Exercice 3 — Barrière de Synchronisation

**Objectif** : N threads doivent tous atteindre un point de rendez-vous avant que l'un d'eux continue.

```
sem_t barrier_sem;   // init = 0
int   count = 0;
sem_t count_lock;    // init = 1

void barrier(int id, int N) {
    sem_wait(&count_lock);
    count++;
    int arrived = count;
    sem_post(&count_lock);

    if (arrived == N) {
        // Dernier arrivé — réveiller les N-1 autres
        for (int i = 0; i < N-1; i++)
            sem_post(&barrier_sem);
    } else {
        // Attendre que le dernier arrive
        sem_wait(&barrier_sem);
    }
    printf("Thread %d passe la barrière\n", id);
}
```

**a)** Trace l'exécution pour N=3 : T0, T1, T2 arrivent dans cet ordre. Quels sem_post/wait sont appelés ?

**b)** Peut-on réutiliser cette barrière (l'appeler une deuxième fois) ? Quel problème se pose ?

**c)** Propose une solution pour rendre la barrière réutilisable.

---

## Exercice 4 — Buffer Borné (taille K)

**a)** Pour un producteur/consommateur avec buffer de taille K, quels sémaphores utiliser ?
Donne le nom, le type (binaire/compteur), et la valeur d'initialisation.

**b)** Que se passe-t-il si on oublie d'appeler `sem_post(empty)` dans le consommateur ?

**c)** Si on a **2 producteurs et 2 consommateurs** avec le même buffer, la solution à 2 sémaphores fonctionne-t-elle encore ? Faut-il ajouter quelque chose ?
(Indice : que se passe-t-il si 2 producteurs font `sem_wait(empty)` simultanément puis écrivent dans `buffer[in++]` ?)

---

## Exercice 5 — Deadlock entre Sémaphores

Voici deux threads :

```c
sem_t S1, S2;
// init : S1=1, S2=1

void *thread1(void *arg) {
    sem_wait(&S1);
    mthread_yield();
    sem_wait(&S2);  // peut bloquer
    // ...
    sem_post(&S2);
    sem_post(&S1);
    return NULL;
}

void *thread2(void *arg) {
    sem_wait(&S2);
    mthread_yield();
    sem_wait(&S1);  // peut bloquer
    // ...
    sem_post(&S1);
    sem_post(&S2);
    return NULL;
}
```

**a)** Trace le scénario de deadlock.

**b)** Identifie les 4 conditions de Coffman.

**c)** Corrige le code en changeant l'ordre d'acquisition dans **un seul** des deux threads.

---

## Corrigés

<details>
<summary>Exercice 2</summary>

a) Avec un seul sémaphore binaire, même les lecteurs s'excluent mutuellement → 1 lecteur à la fois. Perd le bénéfice des lectures simultanées.

b) Dernier lecteur : `sem_post(write_lock)`. Écrivain : `sem_post(write_lock)`.

c) Favorise les lecteurs : si des lecteurs arrivent en continu, `read_count` ne redescend jamais à 0 → les écrivains attendent indéfiniment → famine des écrivains.

</details>

<details>
<summary>Exercice 3</summary>

a) T0 arrive: count=1, sem_wait(barrier). T1 arrive: count=2, sem_wait(barrier). T2 arrive: count=3 (==N) → sem_post×2, puis print. T0 et T1 débloqués → print.

b) Problème : si on réappelle barrier(), `count` repart de 3 (pas remis à 0). Les threads suivants voient count>N → jamais le dernier → deadlock.

c) Remettre count=0 après la barrière (protégé par count_lock), ou utiliser une valeur de génération paire/impaire (barrière à deux phases).

</details>

<details>
<summary>Exercice 4</summary>

a) `sem_empty` (compteur, init=K) + `sem_full` (compteur, init=0).

b) `sem_empty` n'est jamais incrémenté → après K productions, le producteur se bloque indéfiniment → deadlock.

c) Non ! Si 2 producteurs font sem_wait(empty) simultanément, les deux peuvent penser avoir une place. Ils peuvent écrire au même index `in++`. Il faut ajouter un **mutex** autour de l'accès à `buffer[in]` et de l'incrémentation de `in`.

</details>

<details>
<summary>Exercice 5</summary>

a) T1: wait(S1) → S1=0. T2: wait(S2) → S2=0. T1: wait(S2) → bloqué (S2=0). T2: wait(S1) → bloqué (S1=0). Deadlock.

b) 
1. Exclusion mutuelle : S1 et S2 ont valeur max 1
2. Hold-and-wait : T1 tient S1 et attend S2
3. Pas de préemption : ne peut pas retirer S1 de T1
4. Attente circulaire : T1→S2→T2→S1→T1

c) Dans thread2, changer l'ordre : `sem_wait(&S1)` avant `sem_wait(&S2)`. Les deux threads acquièrent S1 en premier → pas d'attente circulaire.

</details>
