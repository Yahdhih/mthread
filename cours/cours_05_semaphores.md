# Cours 05 — Sémaphores

---

## 1. Motivation — Au-delà du mutex

Le mutex ne permet que l'exclusion mutuelle (0 ou 1 thread). Parfois on veut :
- Autoriser **N threads simultanément** (pool de connexions)
- **Signaler** qu'un événement s'est produit (producteur → consommateur)

Le sémaphore généralise le mutex.

---

## 2. Définition du sémaphore

### Définition 2.1 — Sémaphore
Un **sémaphore** est un entier `s >= 0` associé à une file d'attente de threads. Il supporte deux opérations atomiques :

- **`wait(s)`** (aussi noté P, down, sem_wait) :
  ```
  si s > 0 : s -= 1  (et continuer)
  sinon    : bloquer le thread jusqu'à ce que s > 0
  ```

- **`post(s)`** (aussi noté V, up, sem_post) :
  ```
  s += 1
  si des threads attendent : en réveiller un
  ```

### Définition 2.2 — Sémaphore binaire
Un sémaphore initialisé à **1** est équivalent à un mutex. Les seules valeurs possibles sont 0 et 1.

### Définition 2.3 — Sémaphore compteur
Un sémaphore initialisé à **N > 1** permet à N threads de passer simultanément. Exemple : pool de 5 connexions → `sem_init(s, 5)`.

---

## 3. Propriétés formelles

### Théorème 3.1 — Invariant du sémaphore
À tout moment, si `s0` est la valeur initiale, `p` le nombre de `post()` exécutés et `w` le nombre de `wait()` terminés (sans blocage ou débloqués) :

```
s = s0 + p - w >= 0
```

La valeur du sémaphore ne peut jamais être négative.

### Théorème 3.2 — Le sémaphore résout le problème producteur/consommateur
Avec deux sémaphores :
- `empty = N` (nombre de cases vides dans le buffer)
- `full = 0`  (nombre de cases pleines)

```
Producteur :          Consommateur :
  wait(empty)           wait(full)
  produire(item)        consommer(item)
  post(full)            post(empty)
```

Preuve : `empty + full = N` est un invariant → jamais de dépassement.

---

## 4. Structure d'un sémaphore

```c
typedef struct {
    int         value;        // valeur courante (>= 0)
    mthread_t  *waiting_head; // file des threads bloqués
    mthread_t  *waiting_tail;
} mthread_sem_t;
```

### Algorithme de `wait`
```
sem_wait(s) :
  si s.value > 0 :
    s.value -= 1
    retourner
  sinon :
    thread_courant.state = BLOCKED
    ajouter thread_courant à s.waiting_queue
    swapcontext(thread_courant, scheduler)
    // quand on revient ici, s.value a été décrémenté par post()
```

### Algorithme de `post`
```
sem_post(s) :
  si s.waiting_queue est vide :
    s.value += 1
  sinon :
    t = retirer le premier de s.waiting_queue
    t.state = READY
    ajouter t à la run_queue
    // s.value reste à 0 : le crédit va directement au thread réveillé
```

---

## 5. Sémaphore vs Mutex

| Critère          | Mutex                    | Sémaphore                  |
|------------------|--------------------------|----------------------------|
| Valeur initiale  | 1 (binaire)              | N quelconque               |
| Propriétaire     | Oui (seul le locker peut unlock) | Non (n'importe qui peut post) |
| Usage principal  | Exclusion mutuelle       | Signalisation + comptage   |
| Deadlock         | Possible (récursif)      | Possible                   |

**Règle importante** : le mutex a un propriétaire, pas le sémaphore.
```c
// Avec mutex : T1 lock, T1 doit unlock
// Avec sémaphore : T1 peut post même si T2 a fait wait
```

---

## 6. Problèmes classiques résolus par sémaphores

### 6.1 Synchronisation simple
```
T1 fait A, puis T2 fait B (B ne peut commencer qu'après A)

sem_t s = 0;

T1 :              T2 :
  faire A           sem_wait(s)
  sem_post(s)       faire B
```

### 6.2 Barrière (tous les threads doivent arriver avant de continuer)
```
sem_t barriere = 0;
int arrivés = 0;
mutex_t m;

thread_i :
  mutex_lock(m)
  arrivés++
  if arrivés == N : sem_post(barriere)
  mutex_unlock(m)
  sem_wait(barriere)
  sem_post(barriere)   // réveiller le suivant (cascade)
  // continuer
```

---

## Résumé du cours 05

| Concept             | À retenir                                              |
|---------------------|--------------------------------------------------------|
| Sémaphore           | Entier >= 0 avec wait (décrémente) et post (incrémente)|
| wait                | Décrémente ou bloque si 0                              |
| post                | Incrémente et réveille un thread si file non vide      |
| Sémaphore binaire   | Équivalent au mutex (sans notion de propriétaire)      |
| Sémaphore compteur  | Limite N accès simultanés                              |

---

**Précédent** : [Cours 04](cours_04_mutex.md) | **Suivant** : [Cours 06 — Variables de condition](cours_06_cond.md)
