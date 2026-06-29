# Jour 16 — TP04 : mutex_unlock + validation
**Durée : 1h** | Phase 2 — Synchronisation

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Implémenter TODO 4-7 (unlock + trylock) | `mthread_mutex.c` | 30 min |
| Tester | `make && ./test_counter` | 15 min |
| Valgrind | `valgrind --leak-check=full ./test_counter` | 15 min |

## Aide — algorithme de unlock
```
unlock(m):
    si m->wait_queue est vide :
        m->locked = 0
        return
    sinon :
        next = m->wait_queue
        m->wait_queue = next->next
        next->next = NULL
        next->state = MTHREAD_READY
        _mthread_enqueue(next)
        // NE PAS changer m->locked : next "prend" le lock directement
```

## Critère de réussite
`./test_counter` : `counter = 5000 [PASS]`
Valgrind : `0 errors from 0 contexts`

---
➡️ Demain : `day17/` — Cours 05 : Sémaphores
