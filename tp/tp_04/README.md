# TP 04 — Mutex

> Prérequis : TP 03 fonctionnel (mthread_create, yield, join, exit).
> Objectif : implémenter `mthread_mutex_lock` et `mthread_mutex_unlock`.
> Durée estimée : 3h.

---

## Ce que tu vas implémenter

```c
void mthread_mutex_init(mthread_mutex_t *m);
void mthread_mutex_lock(mthread_mutex_t *m);
void mthread_mutex_unlock(mthread_mutex_t *m);
```

---

## Structure du mutex

```c
typedef struct {
    int        locked;        /* 0 = libre, 1 = pris */
    mthread_t  owner;         /* thread qui détient le mutex */
    mthread_t  wait_head;     /* file des threads bloqués */
    mthread_t  wait_tail;
} mthread_mutex_t;
```

---

## Algorithme de lock

```
lock(m) :
  si m.locked == 0 :
    m.locked = 1
    m.owner  = thread_courant
    retourner

  sinon :
    thread_courant.state = BLOCKED
    ajouter thread_courant à m.wait_queue
    swapcontext(thread_courant, scheduler)
    ← quand on revient ici, le mutex nous appartient
```

## Algorithme de unlock

```
unlock(m) :
  si m.wait_queue est vide :
    m.locked = 0
    m.owner  = NULL
  sinon :
    t = retirer premier de m.wait_queue
    t.state = RUNNING  (ou READY + enqueue selon ton design)
    m.owner = t
    ← m.locked reste à 1 : t l'a maintenant
    enqueue(t) ou swapcontext selon le design
```

---

## Tests à valider

### Test 1 — Compteur sans race condition
```
Attendu : compteur == N_THREADS * N_ITER (toujours)
```

### Test 2 — Ordre de libération du mutex (FIFO)
```
Attendu : T1, T2, T3 entrent en section critique dans l'ordre d'arrivée
```

### Test 3 — Double unlock (doit échouer proprement)
```
Attendu : assertion ou message d'erreur, pas de segfault silencieux
```

---

## Commandes

```bash
make
./test_counter    # doit afficher "SUCCÈS" systématiquement
./test_order      # doit afficher T0 T1 T2 en ordre
valgrind --leak-check=full ./test_counter
```
