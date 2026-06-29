# Jour 15 — TP04 : implémenter mthread_mutex_lock
**Durée : 1h** | Phase 2 — Synchronisation

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Lire mthread_mutex.h | comprendre la structure | 10 min |
| Implémenter TODO 1-3 (lock) | `mthread_mutex.c` | 35 min |
| Tester | `make && ./test_counter` | 15 min |

## Aide — algorithme de lock
```
lock(m):
    si m->locked == 0 :
        m->locked = 1
        return
    sinon :
        me = _mthread_current()
        ajouter me en queue de m->wait_queue
        me->state = MTHREAD_BLOCKED
        _mthread_schedule()     ← passer la main
        return                  ← réveillé ici, on détient le lock
```

## Critère de réussite
`./test_counter` affiche un counter final (peut être incorrect si unlock pas encore fait — c'est normal).

---
➡️ Demain : `day16/` — implémenter mutex_unlock et tester
