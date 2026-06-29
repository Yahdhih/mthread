# Cours — Variables de Condition (sections 4-6)

---

## 4. Spurious Wakeup (Réveil Intempestif)

### Définition 4.1 — Spurious Wakeup
Un **spurious wakeup** est un réveil d'un thread bloqué sur `cond_wait` **sans** qu'un `cond_signal` ou `cond_broadcast` ait été appelé.

Ce phénomène est permis par la norme POSIX. Raison : sur certaines architectures ou implémentations, le réveil peut arriver de façon "opportuniste".

### Théorème 4.1 — Règle du `while`
**Toujours** utiliser `while` (jamais `if`) autour d'un `cond_wait` :

```c
// MAUVAIS — vulnérable aux spurious wakeups
mthread_mutex_lock(&mu);
if (count == 0)                    // ← si spurious wakeup → count toujours 0 !
    mthread_cond_wait(&cond, &mu);
consommer();                       // ← accès incorrect
mthread_mutex_unlock(&mu);

// CORRECT
mthread_mutex_lock(&mu);
while (count == 0)                 // ← revérifier la condition au réveil
    mthread_cond_wait(&cond, &mu);
consommer();
mthread_mutex_unlock(&mu);
```

**Deuxième raison du `while`** : avec `broadcast`, **tous** les threads se réveillent, mais un seul peut réellement consommer. Les autres doivent se remettre en attente.

---

## 5. Pattern Monitor

### Définition 5.1 — Monitor
Un **monitor** est une abstraction qui regroupe :
- Des données partagées
- Un mutex (accès exclusif)
- Une ou plusieurs variables de condition (synchronisation)

```c
// Exemple : buffer partagé comme monitor
typedef struct {
    int     data[MAX];
    int     count, in, out;
    mthread_mutex_t mu;
    mthread_cond_t  not_empty;
    mthread_cond_t  not_full;
} monitor_buffer_t;

void monitor_push(monitor_buffer_t *b, int val) {
    mthread_mutex_lock(&b->mu);
    while (b->count == MAX)
        mthread_cond_wait(&b->not_full, &b->mu);
    b->data[b->in++ % MAX] = val;
    b->count++;
    mthread_cond_signal(&b->not_empty);
    mthread_mutex_unlock(&b->mu);
}

int monitor_pop(monitor_buffer_t *b) {
    mthread_mutex_lock(&b->mu);
    while (b->count == 0)
        mthread_cond_wait(&b->not_empty, &b->mu);
    int val = b->data[b->out++ % MAX];
    b->count--;
    mthread_cond_signal(&b->not_full);
    mthread_mutex_unlock(&b->mu);
    return val;
}
```

**Avantages** : interface propre, encapsulation, impossible d'oublier le mutex.

---

## 6. Implémentation dans mthread

### Définition 6.1 — Structure
```c
typedef struct {
    mthread_struct_t *wait_queue;  // threads bloqués sur cette condition
} mthread_cond_t;
```

### Algorithme `cond_wait` (ordre CRITIQUE)
```
cond_wait(c, m):
    me = _mthread_current()
    // 1. Inscrire AVANT de relâcher (évite signal perdu)
    ajouter me à c->wait_queue
    me->state = MTHREAD_BLOCKED
    // 2. Relâcher le mutex
    mthread_mutex_unlock(m)
    // 3. Dormir
    _mthread_schedule()
    // 4. Réveillé → reprendre le mutex avant de retourner
    mthread_mutex_lock(m)
```

### Algorithme `cond_signal`
```
cond_signal(c):
    si c->wait_queue vide : return
    next = retirer premier de c->wait_queue
    _mthread_enqueue(next)
    // Note : next devra réacquérir le mutex via l'étape 4 de cond_wait
```

### Algorithme `cond_broadcast`
```
cond_broadcast(c):
    tant que c->wait_queue non vide :
        next = retirer premier de c->wait_queue
        _mthread_enqueue(next)
```

---

## Résumé complet

| Pattern          | Code                                          |
|------------------|-----------------------------------------------|
| Attendre         | `while (!cond) mthread_cond_wait(&c, &mu);`   |
| Signaler         | `mthread_cond_signal(&c);`                    |
| Signaler tous    | `mthread_cond_broadcast(&c);`                 |
| `if` vs `while`  | Toujours `while` — spurious wakeup et broadcast |
| Ordre cond_wait  | queue → unlock → sleep → lock au réveil        |
