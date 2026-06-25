# TD 05 — Sémaphores

> Exercices sur papier. Durée estimée : 2h.

---

## Exercice 1 — Traces de sémaphores

Trace la valeur du sémaphore `s` (initialisé à 2) et l'état des threads à chaque étape :

```
s = 2

T1 : wait(s) → attendre 1s → post(s)
T2 : wait(s) → attendre 1s → post(s)
T3 : wait(s) → attendre 1s → post(s)
T4 : wait(s) → attendre 1s → post(s)

Tous les threads démarrent en même temps.
```

| Temps | Événement          | Valeur s | T1    | T2    | T3    | T4    |
|-------|--------------------|----------|-------|-------|-------|-------|
| t=0   | T1,T2,T3,T4 wait   | ?        | ?     | ?     | ?     | ?     |
| ...   | ...                | ...      | ...   | ...   | ...   | ...   |

Complète le tableau.

---

## Exercice 2 — Sémaphore vs Mutex

a) Peut-on implémenter un mutex avec un sémaphore ? Si oui, comment ? Quelle propriété du mutex ne peut pas être reproduite exactement ?

b) Peut-on implémenter un sémaphore avec un mutex ? Donne le pseudo-code.

c) Un thread T1 fait `sem_wait(s)` ; le thread T2 fait `sem_post(s)`. Est-ce valide si T2 n'a jamais fait de `sem_wait` ? Comparer avec un mutex.

---

## Exercice 3 — Modélisation avec sémaphores

Pour chaque problème, propose une solution avec des sémaphores (sans mutex). Donne le pseudo-code pour chaque thread.

**Problème A — Synchronisation simple :**
T2 ne doit pas commencer B tant que T1 n'a pas fini A.

**Problème B — Rendez-vous :**
T1 et T2 doivent tous deux atteindre un point avant que l'un ou l'autre continue.

**Problème C — Accès limité :**
Au plus 3 threads peuvent être dans la fonction `section_critique()` simultanément.

**Problème D — Barrière générale :**
N threads doivent tous arriver à un point avant que quiconque continue. (Hint : utiliser un compteur protégé + un sémaphore)

---

## Exercice 4 — Le problème du dîner des philosophes

5 philosophes sont assis en cercle. Entre chaque paire de philosophes il y a une fourchette (5 fourchettes en tout). Pour manger, un philosophe a besoin des **deux** fourchettes adjacentes.

```
        F0
   P4       P0
F4              F1
   P3       P1
        F2  F3
           P2
```

**Solution naïve (bogué) :**
```
philosophe(i) :
  while (1) :
    penser()
    prendre fourchette gauche  [i]
    prendre fourchette droite  [(i+1) % 5]
    manger()
    poser fourchette gauche
    poser fourchette droite
```

a) Montre que cette solution peut mener à un deadlock (tous les philosophes prennent leur fourchette gauche en même temps).

b) Propose **deux solutions différentes** qui évitent le deadlock. Pour chaque solution, explique pourquoi le deadlock est impossible.

---

## Exercice 5 — Producteur / Consommateur avec N producteurs et M consommateurs

```c
sem_t empty = N;   // N = capacité du buffer
sem_t full  = 0;
sem_t mutex = 1;   // pour protéger le buffer lui-même

int buffer[N];
int in = 0, out = 0;
```

a) Complète le pseudo-code du producteur et du consommateur.

b) Pourquoi a-t-on besoin du sémaphore `mutex` en plus de `empty` et `full` ?

c) Si on supprime `mutex`, donne un exemple d'interleaving qui corromprait le buffer.

d) Peut-on remplacer `mutex` par un vrai mutex `mthread_mutex_t` ? Y a-t-il un avantage ?

---

## Corrigé

<details>
<summary>Exercice 1 — Trace</summary>

```
t=0  : T1.wait → s=1 (T1 passe), T2.wait → s=0 (T2 passe), T3.wait → s<0 bloqué, T4.wait → bloqué
       T1: RUNNING, T2: RUNNING, T3: BLOCKED, T4: BLOCKED

t=1  : T1.post → s=0, T3 réveillé → READY (puis RUNNING)
       T2.post → s=0 (si T4 attendait), T4 réveillé
       ...
```

Conclusion : avec s=2, seulement 2 threads au maximum sont dans la section en même temps.

</details>

<details>
<summary>Exercice 3</summary>

**A :**
```
sem s = 0
T1 : faire_A() ; post(s)
T2 : wait(s)   ; faire_B()
```

**B :**
```
sem s1 = 0, s2 = 0
T1 : arriver() ; post(s2) ; wait(s1) ; continuer()
T2 : arriver() ; post(s1) ; wait(s2) ; continuer()
```

**C :**
```
sem s = 3
thread_i : wait(s) ; section_critique() ; post(s)
```

</details>

<details>
<summary>Exercice 4 — Philosophes</summary>

**Solution 1 — Ordre global** : un philosophe (ex: P4) prend d'abord la fourchette droite, pas la gauche. Brise le cycle.

**Solution 2 — Sémaphore global** : un sémaphore initialisé à 4 (max 4 philosophes peuvent tenter de prendre des fourchettes). Garantit qu'au moins un philosophe peut manger.

</details>
