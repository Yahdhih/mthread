# Jour 31 — Projet : consolider mthread.c
**Durée : 1h** | Phase 4 — Projet Final

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Copier la meilleure version de mthread.c | `src/mthread.c` | 15 min |
| Vérifier que exit + join fonctionnent | `make && ./tests/test_threads` | 30 min |
| Corriger les bugs | | 15 min |

## Critère de réussite
`tests/test_threads` : `[PASS]`
`valgrind tests/test_threads` : `0 errors`

## Rappel de la solution exit/join
```c
void mthread_exit(void *retval) {
    current->state  = MTHREAD_ZOMBIE;
    current->retval = retval;
    if (current->joiner) {
        current->joiner->state = MTHREAD_READY;
        enqueue(current->joiner);
        current->joiner = NULL;
    }
    setcontext(&sched_ctx);
}

int mthread_join(mthread_t thread, void **retval) {
    if (thread->state != MTHREAD_ZOMBIE) {
        current->state = MTHREAD_BLOCKED;
        thread->joiner = current;
        swapcontext(&current->ctx, &sched_ctx);
    }
    if (retval) *retval = thread->retval;
    free(thread->stack);
    free(thread);
    return 0;
}
```

---
➡️ Demain : `day32/` — Mutex final + test_mutex
