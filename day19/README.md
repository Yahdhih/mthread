# Jour 19 — TP05 : implémenter mthread_sem
**Durée : 1h** | Phase 2 — Synchronisation

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Lire mthread_sem.h | comprendre la structure | 5 min |
| Implémenter sem_init + sem_wait | `mthread_sem.c` | 30 min |
| Implémenter sem_post | `mthread_sem.c` | 15 min |
| Tester | `make && ./test_prod_conso` | 10 min |

## Aide — algorithme sem_wait
```
sem_wait(s):
    si s->value > 0 :
        s->value--
        return
    sinon :
        me = _mthread_current()
        ajouter me à s->wait_queue
        me->state = MTHREAD_BLOCKED
        _mthread_schedule()
        return   ← réveillé quand un sem_post libère un jeton
```

## Aide — algorithme sem_post
```
sem_post(s):
    si s->wait_queue vide :
        s->value++
    sinon :
        next = retirer de s->wait_queue
        _mthread_enqueue(next)
        // NE PAS incrémenter value
```

## Critère de réussite
`./test_prod_conso` : aucun deadlock, tous les items consommés, `[PASS]`

---
➡️ Demain : `day20/` — TP05 validation + début Cours 06
