# Cours — Exclusion Mutuelle

---

## 1. Race Condition (Condition de Course)

### Définition 1.1 — Race Condition
Une **race condition** se produit quand le résultat d'un calcul dépend de l'ordre d'entrelacement des opérations de plusieurs threads.

**Exemple classique : `counter++`**

```c
// T1 et T2 exécutent simultanément :
counter = counter + 1;
```

En assembleur, `counter++` se décompose en **3 instructions** :
```
LOAD  rax, [counter]   ; lire la valeur
ADD   rax, 1           ; incrémenter
STORE [counter], rax   ; écrire le résultat
```

**Entrelacement fatal** (counter vaut 0 au départ) :

```
T1: LOAD  rax=0
T1: ADD   rax=1
                    T2: LOAD  rax=0   ← lit l'ancienne valeur !
                    T2: ADD   rax=1
T1: STORE [counter]=1
                    T2: STORE [counter]=1  ← écrase le résultat de T1 !
Résultat : 1 au lieu de 2
```

---

## 2. Section Critique

### Définition 2.1 — Section Critique
Une **section critique** est une portion de code qui accède à une ressource partagée et qui ne doit être exécutée que par **un seul thread à la fois**.

### Théorème 2.1 — Propriétés d'une bonne solution
Une solution correcte pour les sections critiques doit satisfaire **simultanément** :

1. **Exclusion mutuelle** : au plus un thread dans la section critique à tout instant
2. **Progrès** : si aucun thread n'est dans la SC, un thread qui veut y entrer peut le faire
3. **Attente bornée** : un thread ne peut pas attendre indéfiniment (pas de famine)
4. **Absence d'hypothèse sur la vitesse** : la solution ne doit pas dépendre de la vitesse relative des threads

---

## 3. Algorithme de Peterson (2 threads)

```c
// Variables partagées
int turn;           // à qui le tour
int want[2] = {0, 0};  // thread i veut entrer ?

// Thread i (i=0 ou i=1, j=1-i)
void lock(int i) {
    int j = 1 - i;
    want[i] = 1;    // je veux entrer
    turn = j;       // je cède la priorité à l'autre
    while (want[j] && turn == j)
        ;           // attente active
}

void unlock(int i) {
    want[i] = 0;    // je sors
}
```

**Preuve informelle** :
- Si T0 et T1 veulent entrer : le dernier à avoir fait `turn = j` cède. L'autre entre.
- Un seul entre à la fois → exclusion mutuelle.
- Si T1 ne veut pas (`want[1]=0`), T0 entre immédiatement → pas de blocage inutile.

**Limitation** : fonctionne seulement pour 2 threads. Généraliser est compliqué.

---

## 4. Opérations Atomiques

### Définition 4.1 — Atomicité
Une opération est **atomique** si elle est indivisible : aucun autre thread ne peut l'interrompre à mi-chemin.

**Compare-and-Swap (CAS)** — l'instruction matérielle clé :
```c
// Pseudocode de CAS
int compare_and_swap(int *addr, int expected, int new_val) {
    // ATOMIQUE :
    if (*addr == expected) {
        *addr = new_val;
        return 1;  // succès
    }
    return 0;  // échec
}
```

En GCC :
```c
__atomic_compare_exchange_n(&locked, &expected, 1, 0,
                            __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
```

**Spinlock basé sur CAS** :
```c
void spin_lock(int *locked) {
    int expected = 0;
    while (!__atomic_compare_exchange_n(locked, &expected, 1, 0,
           __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
        expected = 0;  // réinitialiser pour le prochain essai
    }
}
```

---

## 5. Le Mutex

### Définition 5.1 — Mutex (Mutual Exclusion Lock)
Un **mutex** est un verrou qui garantit l'exclusion mutuelle. Il a deux états : **libre** (0) ou **verrouillé** (1).

**Structure** :
```c
typedef struct {
    int locked;               // 0=libre, 1=verrouillé
    mthread_struct_t *wait_queue;  // threads en attente
} mthread_mutex_t;
```

**Algorithme `mutex_lock`** :
```
lock(m):
    si m->locked == 0:
        m->locked = 1
        retourner
    sinon:
        ajouter current thread à m->wait_queue
        current->state = BLOCKED
        yield vers scheduler
        // réveillé par unlock → le thread détient maintenant le lock
```

**Algorithme `mutex_unlock`** :
```
unlock(m):
    si m->wait_queue est vide:
        m->locked = 0
        retourner
    sinon:
        next = retirer premier de m->wait_queue
        next->state = READY
        enqueue(next)
        // NE PAS changer m->locked : next "hérite" du lock
```

---

## 6. Deadlock

### Définition 6.1 — Deadlock (Interblocage)
Un **deadlock** est une situation où un ensemble de threads est bloqué indéfiniment, chacun attendant une ressource détenue par un autre.

### Théorème 3.1 — Conditions de Coffman (1971)
Un deadlock ne peut survenir que si les **4 conditions suivantes** sont simultanément vraies :

1. **Exclusion mutuelle** : au moins une ressource n'est pas partageable
2. **Hold-and-wait** : un thread détient une ressource en attendant une autre
3. **Pas de préemption** : une ressource ne peut être retirée de force à un thread
4. **Attente circulaire** : T1 attend une ressource de T2, T2 attend une ressource de T1

**Pour éviter le deadlock : briser au moins une condition.**

**Exemple classique** :
```c
// T1                    // T2
lock(&mutex_A);          lock(&mutex_B);
lock(&mutex_B);  ←←←←  lock(&mutex_A);  ← DEADLOCK possible
```

**Solution simple** : toujours acquérir les locks dans le **même ordre** :
```c
// T1 ET T2 font toujours :
lock(&mutex_A);
lock(&mutex_B);
```

---

## 7. Loi d'Amdahl

### Théorème 4.1 — Loi d'Amdahl (1967)
Si une fraction **f** du programme est séquentielle (non parallélisable), l'accélération maximale avec **p** processeurs est :

$$S(p) = \frac{1}{f + \frac{1-f}{p}}$$

**Exemples** (f = 10% séquentiel) :

| p (processeurs) | S(p) |
|-----------------|------|
| 1               | 1.0× |
| 2               | 1.82× |
| 4               | 3.08× |
| 8               | 4.71× |
| ∞               | 10×  |

**Conclusion** : même avec ∞ processeurs, on est limité par la partie séquentielle. Si f=0.1, le gain maximum est **10×** quelles que soient les ressources.

---

## Résumé

| Concept          | À retenir                                              |
|------------------|--------------------------------------------------------|
| Race condition   | Résultat dépend de l'ordre d'entrelacement             |
| Section critique | Code accédant au partagé — un seul thread à la fois   |
| Mutex            | Lock/unlock avec file d'attente (pas d'attente active) |
| Deadlock         | 4 conditions de Coffman — briser l'une pour l'éviter  |
| Amdahl           | Accélération bornée par la fraction séquentielle       |
