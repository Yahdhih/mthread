# TP 05 — Sémaphores

> Prérequis : TP 04 fonctionnel.
> Objectif : implémenter `mthread_sem_wait` et `mthread_sem_post`.
> Durée estimée : 2h.

---

## Ce que tu vas implémenter

```c
void mthread_sem_init(mthread_sem_t *s, int value);
void mthread_sem_wait(mthread_sem_t *s);
void mthread_sem_post(mthread_sem_t *s);
```

---

## Structure du sémaphore

```c
typedef struct {
    int        value;
    mthread_t  wait_head;
    mthread_t  wait_tail;
} mthread_sem_t;
```

## Algorithme

```
sem_wait(s) :
  si s.value > 0 :
    s.value -= 1
    retourner
  sinon :
    bloquer le thread courant (BLOCKED + file d'attente)
    swapcontext → scheduler

sem_post(s) :
  si file vide :
    s.value += 1
  sinon :
    réveiller le premier thread en attente (READY + run_queue)
    // s.value reste à 0 : le crédit va au thread réveillé
```

---

## Tests à valider

### Test 1 — Pool de ressources (sémaphore compteur = 3)
Au plus 3 threads actifs simultanément parmi 10.

### Test 2 — Synchronisation simple
T2 attend que T1 ait fini son travail (sem initialisé à 0).

### Test 3 — Producteur/Consommateur
1 producteur, 1 consommateur, buffer de taille 5.

```bash
make test
```
