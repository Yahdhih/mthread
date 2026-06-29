# Jour 11 — TP03 : mthread_yield + mthread_create (TODO 8-14)
**Durée : 1h** | Phase 2 — Bibliothèque mthread

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Implémenter TODO 8-14 dans `mthread.c` | `mthread.c` | 1h |

## Ce que tu dois implémenter aujourd'hui

- **TODO 8** : allouer `mthread_struct_t` dans `mthread_create`
- **TODO 9** : remplir `id`, `state=READY`, `func`, `arg`
- **TODO 10** : allouer la pile, `getcontext`, configurer `uc_stack`, `makecontext`
- **TODO 11** : `enqueue` le thread
- **TODO 12** : initialiser `scheduler_ctx` (la première fois seulement)
- **TODO 13** : dans `mthread_yield` — remettre `current` en READY + enqueue
- **TODO 14** : dans `mthread_yield` — `swapcontext` vers scheduler

## Note du prof

Les TODO de `mthread_create` (8-12) sont les plus longs. Voici le squelette de `getcontext`/`makecontext` pour une pile :

```c
t->stack = malloc(STACK_SIZE);
getcontext(&t->ctx);
t->ctx.uc_stack.ss_sp   = t->stack;
t->ctx.uc_stack.ss_size = STACK_SIZE;
t->ctx.uc_link          = NULL;   // on gèrera avec mthread_exit
makecontext(&t->ctx, thread_entry, 0);
```

Pour le `scheduler_ctx` : utilise `getcontext(&scheduler_ctx)` au moment de la première création, et lance le scheduler avec `swapcontext` depuis `mthread_yield` si nécessaire. Une alternative plus propre : initialiser le scheduler dans `mthread_yield` quand `current == NULL`.

Quand TODO 8-14 sont faits, `make && ./test_yield` devrait afficher les threads en alternance.

---

➡️ Demain : `day12/` — TP03 : mthread_exit + mthread_join (TODO 15-22)
