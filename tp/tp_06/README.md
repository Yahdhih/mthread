# TP 06 — Variables de condition

> Prérequis : TP 04 (mutex).
> Objectif : implémenter `mthread_cond_wait`, `mthread_cond_signal`, `mthread_cond_broadcast`.
> Durée estimée : 3h.

---

## Ce que tu vas implémenter

```c
void mthread_cond_init(mthread_cond_t *c);
void mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m);
void mthread_cond_signal(mthread_cond_t *c);
void mthread_cond_broadcast(mthread_cond_t *c);
```

---

## Point critique — Atomicité de `cond_wait`

`cond_wait` doit **atomiquement** libérer le mutex et se bloquer.

Dans notre scheduler coopératif, l'atomicité est garantie naturellement : puisqu'on ne cède le CPU qu'explicitement, personne ne peut nous interrompre entre `unlock` et `swapcontext`.

```c
void mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m) {
    // 1. S'enregistrer dans la file d'attente de la condition
    // 2. Libérer le mutex       ← aucun thread ne peut s'intercaler ici
    // 3. Se bloquer (swapcontext vers scheduler)
    // 4. Quand réveillé : réacquérir le mutex
}
```

---

## Tests à valider

### Test 1 — Producteur/Consommateur (sans sémaphore)
Reimplémenter le problème avec mutex + cond.

### Test 2 — Barrière
N threads attendent tous au même point.

### Test 3 — Signal vs Broadcast
Montre la différence : avec `signal`, un seul thread est réveillé.

```bash
make test
```
