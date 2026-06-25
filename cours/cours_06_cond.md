# Cours 06 — Variables de Condition

---

## 1. Motivation — La limite du mutex seul

Le mutex garantit l'exclusion mutuelle, mais il ne permet pas à un thread d'**attendre qu'une condition soit vraie** sans gaspiller le CPU.

### Mauvaise solution — Attente active (busy-wait)
```c
mutex_lock(&m);
while (buffer_vide()) {      // ← tourne en boucle, gaspille le CPU
    mutex_unlock(&m);
    mutex_lock(&m);
}
consommer();
mutex_unlock(&m);
```
C'est inefficace : le thread monopolise le CPU à ne rien faire.

### Solution — Variable de condition
Une **variable de condition** permet à un thread de se **suspendre** en attendant qu'une condition soit remplie, et d'être **réveillé** quand un autre thread la satisfait.

---

## 2. Les trois opérations

### Définition 2.1 — `mthread_cond_wait`
```c
void mthread_cond_wait(mthread_cond_t *cond, mthread_mutex_t *mutex);
```
**Atomiquement** :
1. Libère le `mutex`
2. Bloque le thread courant sur `cond`

Quand le thread est réveillé :
3. Réacquiert le `mutex` avant de retourner

**L'atomicité est critique** : si libérer le mutex et se bloquer n'étaient pas atomiques, un signal pourrait être perdu entre les deux.

### Définition 2.2 — `mthread_cond_signal`
```c
void mthread_cond_signal(mthread_cond_t *cond);
```
Réveille **un** thread en attente sur `cond`. Si aucun thread n'attend, l'appel est sans effet (le signal est perdu).

### Définition 2.3 — `mthread_cond_broadcast`
```c
void mthread_cond_broadcast(mthread_cond_t *cond);
```
Réveille **tous** les threads en attente sur `cond`. Chacun devra réacquérir le mutex l'un après l'autre.

---

## 3. Le patron d'utilisation

### Théorème 3.1 — Pattern obligatoire : `while`, pas `if`
```c
// CORRECT
mutex_lock(&m);
while (!condition_vraie()) {      // ← TOUJOURS while
    cond_wait(&cond, &m);
}
// utiliser la ressource
mutex_unlock(&m);

// DANGEREUX
mutex_lock(&m);
if (!condition_vraie()) {         // ← if : risque de spurious wakeup
    cond_wait(&cond, &m);
}
mutex_unlock(&m);
```

### Définition 3.1 — Spurious wakeup (réveil intempestif)
Un thread peut être réveillé **sans qu'un signal ait été envoyé**. C'est permis par POSIX (et arrive sur Linux). Le `while` protège contre ce cas.

---

## 4. Problème producteur/consommateur complet

```c
#define CAPACITY 10

int    buffer[CAPACITY];
int    count = 0;          // nombre d'éléments dans le buffer
mthread_mutex_t mutex  = MTHREAD_MUTEX_INIT;
mthread_cond_t  non_plein  = MTHREAD_COND_INIT;
mthread_cond_t  non_vide   = MTHREAD_COND_INIT;

void *producteur(void *arg) {
    for (int i = 0; i < 100; i++) {
        int item = produire(i);

        mutex_lock(&mutex);
        while (count == CAPACITY)           // buffer plein → attendre
            cond_wait(&non_plein, &mutex);

        buffer[count++] = item;
        cond_signal(&non_vide);             // signaler : buffer non vide
        mutex_unlock(&mutex);
    }
    return NULL;
}

void *consommateur(void *arg) {
    for (int i = 0; i < 100; i++) {
        mutex_lock(&mutex);
        while (count == 0)                  // buffer vide → attendre
            cond_wait(&non_vide, &mutex);

        int item = buffer[--count];
        cond_signal(&non_plein);            // signaler : buffer non plein
        mutex_unlock(&mutex);

        consommer(item);
    }
    return NULL;
}
```

---

## 5. Structure d'une variable de condition

```c
typedef struct {
    mthread_t *waiting_head;   // file des threads en attente
    mthread_t *waiting_tail;
} mthread_cond_t;
```

### Algorithme de `cond_wait`
```
cond_wait(cond, mutex) :
  1. ajouter thread_courant à cond.waiting_queue
  2. thread_courant.state = BLOCKED
  3. mutex_unlock(mutex)            ← libérer le mutex AVANT de bloquer
  4. swapcontext(thread_courant, scheduler)
  // ici le thread est réveillé par signal/broadcast
  5. mutex_lock(mutex)              ← réacquérir le mutex
```

### Algorithme de `cond_signal`
```
cond_signal(cond) :
  si cond.waiting_queue non vide :
    t = retirer premier de cond.waiting_queue
    t.state = READY
    ajouter t à run_queue
    // t devra encore acquérir le mutex (étape 5 de cond_wait)
```

---

## 6. `signal` vs `broadcast` — Quand choisir ?

| Situation                                   | Utiliser            |
|---------------------------------------------|---------------------|
| Un seul thread peut utiliser la ressource   | `signal`            |
| Plusieurs threads peuvent progresser        | `broadcast`         |
| La condition dépend du thread réveillé      | `broadcast`         |
| Doute                                       | `broadcast` (plus sûr) |

**Exemple où `broadcast` est nécessaire** :
```c
// Barrière : tous les threads attendent que tous soient arrivés
while (arrivés < N)
    cond_wait(&cond, &mutex);
cond_broadcast(&cond);  // réveiller TOUS les threads bloqués
```

---

## 7. Le Monitor — Patron de conception

### Définition 7.1 — Monitor
Un **monitor** est un objet qui encapsule des données partagées + un mutex + des variables de condition. Toutes les opérations sur l'objet acquièrent automatiquement le mutex.

```c
typedef struct {
    int              data[SIZE];
    int              count;
    mthread_mutex_t  lock;
    mthread_cond_t   not_empty;
    mthread_cond_t   not_full;
} Monitor;

void monitor_put(Monitor *m, int val) {
    mutex_lock(&m->lock);
    while (m->count == SIZE) cond_wait(&m->not_full, &m->lock);
    m->data[m->count++] = val;
    cond_signal(&m->not_empty);
    mutex_unlock(&m->lock);
}

int monitor_get(Monitor *m) {
    mutex_lock(&m->lock);
    while (m->count == 0) cond_wait(&m->not_empty, &m->lock);
    int val = m->data[--m->count];
    cond_signal(&m->not_full);
    mutex_unlock(&m->lock);
    return val;
}
```

---

## Résumé du cours 06

| Concept             | À retenir                                                   |
|---------------------|-------------------------------------------------------------|
| `cond_wait`         | Libère le mutex et bloque — atomiquement                    |
| `cond_signal`       | Réveille un thread en attente                               |
| `cond_broadcast`    | Réveille tous les threads en attente                        |
| Spurious wakeup     | Réveil sans signal → toujours utiliser `while`              |
| Monitor             | Encapsulation mutex + variables de condition                |

---

**Précédent** : [Cours 05](cours_05_semaphores.md) | **Suivant** : [Cours 07 — Scheduling](cours_07_scheduling.md)
