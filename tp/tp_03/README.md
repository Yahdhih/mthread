# TP 03 — Création et cycle de vie des threads

> Objectif : implémenter `mthread_create`, `mthread_yield`, `mthread_join`, `mthread_exit`.
> C'est le coeur de la bibliothèque. Prends le temps de bien comprendre chaque fonction.
> Durée estimée : 4h.

---

## Ce que tu vas implémenter

Fichier à compléter : `mthread.c` / `mthread.h`

```c
int  mthread_create(mthread_t *tid, void *(*func)(void *), void *arg);
void mthread_yield(void);
int  mthread_join(mthread_t tid, void **retval);
void mthread_exit(void *retval);
```

---

## Architecture à suivre

```
mthread_create()
    → alloue struct mthread + pile
    → initialise le contexte (getcontext + makecontext)
    → uc_link = &scheduler.ctx    ← pour récupérer la main quand func() retourne
    → state = READY
    → enqueue(run_queue, thread)

mthread_yield()
    → current->state = READY
    → enqueue(run_queue, current)
    → swapcontext(&current->ctx, &scheduler.ctx)

scheduler_run()   ← boucle principale, jamais appelée par l'utilisateur
    → next = dequeue(run_queue)
    → next->state = RUNNING
    → current = next
    → swapcontext(&scheduler.ctx, &next->ctx)

mthread_exit(retval)
    → current->retval = retval
    → current->state = ZOMBIE
    → si quelqu'un attend en join() sur ce thread → le réveiller
    → setcontext(&scheduler.ctx)   ← ne revient pas

mthread_join(tid, retval)
    → si tid est déjà ZOMBIE → récupérer retval, libérer, retourner
    → sinon : marquer current comme waiter de tid
    → current->state = BLOCKED
    → swapcontext(&current->ctx, &scheduler.ctx)
    → quand réveillé : tid est ZOMBIE, récupérer retval, libérer
```

---

## Étapes recommandées

### Étape 1 — Structures de données (mthread.h)
Définis `mthread_t` et `scheduler_t`.

### Étape 2 — `mthread_create` + scheduler simple
Fais tourner un seul thread qui retourne.

### Étape 3 — `mthread_yield`
Fais alterner 3 threads via `yield`.

### Étape 4 — `mthread_exit` + `mthread_join`
Test : thread parent attend que l'enfant finisse.

---

## Tests à valider (dans `tests/`)

```bash
make test
```

- `test_single` : un seul thread qui retourne 42, main récupère 42
- `test_yield`  : 3 threads qui s'alternent 5 fois
- `test_join`   : thread parent attend résultat enfant
- `test_chain`  : T1 crée T2, T2 crée T3, chacun join le suivant
