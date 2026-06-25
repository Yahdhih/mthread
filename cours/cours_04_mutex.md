# Cours 04 — Mutex et Exclusion Mutuelle

---

## 1. Le problème : la race condition

### Définition 1.1 — Race condition (condition de course)
Une **race condition** se produit quand le résultat d'un programme dépend de l'**ordre d'exécution** des threads, et que cet ordre n'est pas contrôlé.

### Exemple classique — compteur partagé
```c
int compteur = 0;   // variable partagée

void *incrementer(void *arg) {
    for (int i = 0; i < 100000; i++) {
        compteur++;       // ← PAS atomique !
    }
    return NULL;
}
```

`compteur++` se décompose en **3 instructions assembleur** :
```asm
MOV rax, [compteur]   ; 1. charger la valeur depuis la mémoire
ADD rax, 1            ; 2. incrémenter dans le registre
MOV [compteur], rax   ; 3. écrire la nouvelle valeur en mémoire
```

Si T1 et T2 s'interlevent entre les instructions 1 et 3 :
```
T1: LOAD rax ← 5
T2: LOAD rax ← 5    ← lit 5, pas 6 !
T1: ADD rax = 6
T2: ADD rax = 6
T1: STORE compteur = 6
T2: STORE compteur = 6  ← écrase la valeur de T1 !
Résultat attendu : 7, résultat obtenu : 6
```

### Définition 1.2 — Section critique
Une **section critique** est une portion de code qui accède à une ressource partagée et qui **ne doit être exécutée que par un seul thread à la fois**.

```c
// Section critique : la modification de compteur
compteur++;
```

---

## 2. Le mutex

### Définition 2.1 — Mutex (Mutual Exclusion)
Un **mutex** est un verrou binaire qui garantit qu'un seul thread à la fois peut exécuter la section critique. Il a deux états : **verrouillé** (locked) et **déverrouillé** (unlocked).

```c
mthread_mutex_t mutex = MTHREAD_MUTEX_INIT;

void *incrementer(void *arg) {
    for (int i = 0; i < 100000; i++) {
        mthread_mutex_lock(&mutex);    // ← entrer en section critique
        compteur++;                     // section critique
        mthread_mutex_unlock(&mutex);  // ← sortir de la section critique
    }
    return NULL;
}
```

### Définition 2.2 — `mthread_mutex_lock`
```c
void mthread_mutex_lock(mthread_mutex_t *m);
```
- Si le mutex est **libre** : le verrouiller et continuer
- Si le mutex est **pris** : bloquer le thread courant (état BLOCKED) et le mettre en file d'attente du mutex

### Définition 2.3 — `mthread_mutex_unlock`
```c
void mthread_mutex_unlock(mthread_mutex_t *m);
```
- Libère le mutex
- Si des threads attendaient : en réveille un (le passe à READY)

---

## 3. Propriétés fondamentales

### Théorème 3.1 — Exclusion mutuelle
Si un thread T1 est dans la section critique protégée par un mutex M, alors **aucun autre thread** ne peut entrer dans cette section critique.

### Théorème 3.2 — Absence de famine (Fairness)
Un thread qui veut acquérir un mutex **finira par l'obtenir** en temps fini, à condition que les autres threads libèrent le mutex.

### Théorème 3.3 — Deadlock (Interblocage)
Un **deadlock** se produit quand deux threads s'attendent mutuellement pour jamais :

```
T1 : lock(A) ──── attend lock(B)
                              │
T2 : lock(B) ──── attend lock(A)
```
Les deux threads se bloquent indéfiniment. **Il n'y a pas de sortie.**

**Condition nécessaire** (conditions de Coffman) pour un deadlock :
1. Exclusion mutuelle (les ressources ne sont pas partageables)
2. Possession et attente (un thread tient une ressource et en attend une autre)
3. Pas de préemption (on ne peut pas forcer un thread à libérer)
4. Attente circulaire (T1 attend T2 qui attend T1)

**Règle de prévention** : toujours acquérir les mutex dans le **même ordre** dans tous les threads.

---

## 4. Structure d'un mutex

```c
typedef struct {
    int           locked;        // 0 = libre, 1 = pris
    mthread_t    *owner;         // thread qui détient le mutex
    mthread_t    *waiting_head;  // file des threads bloqués
    mthread_t    *waiting_tail;
} mthread_mutex_t;
```

### Algorithme de lock
```
lock(m) :
  si m.locked == 0 :
    m.locked = 1
    m.owner = thread_courant
    retourner
  sinon :
    thread_courant.state = BLOCKED
    ajouter thread_courant à m.waiting_queue
    swapcontext(thread_courant, scheduler)  ← céder le CPU
```

### Algorithme de unlock
```
unlock(m) :
  si m.waiting_queue est vide :
    m.locked = 0
    m.owner = NULL
  sinon :
    t = retirer le premier de m.waiting_queue
    t.state = READY
    ajouter t à la run_queue  ← il pourra acquérir le mutex
    (ne pas changer m.locked : le prochain qui sort de BLOCKED l'aura)
```

---

## 5. Mutex récursif vs non-récursif

### Définition 5.1 — Mutex non-récursif (par défaut)
Si un thread **déjà propriétaire** d'un mutex tente de le verrouiller à nouveau → **deadlock immédiat**.

### Définition 5.2 — Mutex récursif
Un mutex récursif peut être verrouillé plusieurs fois par le **même thread**. Un compteur interne suit le nombre d'acquisitions. Il faut autant de `unlock()` que de `lock()`.

---

## 6. Erreurs classiques

### Erreur 1 — Oubli du unlock
```c
mutex_lock(&m);
if (erreur) return;   // ← unlock oublié ! mutex jamais libéré
mutex_unlock(&m);
```

### Erreur 2 — Protéger le mauvais objet
```c
int a = 0, b = 0;
mthread_mutex_t m_a, m_b;

// T1                   // T2
mutex_lock(&m_a);        mutex_lock(&m_b);
b++;                     a++;    // ← b et a ne sont pas protégés par leur mutex
mutex_unlock(&m_a);      mutex_unlock(&m_b);
```

### Erreur 3 — Section critique trop large
```c
mutex_lock(&m);
calcul_long_3_secondes();  // ← les autres threads attendent 3 secondes !
shared_var++;
mutex_unlock(&m);

// Mieux :
calcul_long_3_secondes();
mutex_lock(&m);
shared_var++;
mutex_unlock(&m);
```

---

## Résumé du cours 04

| Concept            | À retenir                                              |
|--------------------|--------------------------------------------------------|
| Race condition     | Résultat dépend de l'ordre d'exécution non contrôlé    |
| Section critique   | Code à exécuter par un seul thread à la fois           |
| Mutex              | Verrou binaire pour protéger une section critique      |
| Deadlock           | Deux threads s'attendent mutuellement → blocage infini |
| Prévention         | Acquérir les mutex dans le même ordre                  |

---

**Précédent** : [Cours 03](cours_03_cycle_vie.md) | **Suivant** : [Cours 05 — Sémaphores](cours_05_semaphores.md)
