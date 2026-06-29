# Jour 10 — TP03 : mthread_create et scheduler (TODO 1-7)
**Durée : 1h** | Phase 2 — Bibliothèque mthread

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Implémenter TODO 1-7 dans `mthread.c` | `mthread.c` | 1h |

## Ce que tu dois implémenter aujourd'hui

- **TODO 1** : `thread_entry` — wrapper qui appelle `func(arg)` puis `mthread_exit`
- **TODO 2** : même TODO 1 (appel de `func`)
- **TODO 3** : même TODO 1 (appel de `mthread_exit`)
- **TODO 4** : `enqueue` — ajouter un thread en queue de `ready_queue`
- **TODO 5** : `dequeue` — retirer la tête de `ready_queue`
- **TODO 6** : `scheduler_run` — fin de boucle quand file vide
- **TODO 7** : `scheduler_run` — swap vers le prochain thread

## Note du prof

Commence par implémenter `enqueue` et `dequeue` (TODO 4 et 5) — ils sont indépendants et faciles à tester mentalement. Ensuite `scheduler_run` (TODO 6 et 7). Enfin `thread_entry` (TODO 1-3).

Aujourd'hui tu n'implémenteras pas `mthread_create` (TODO 8-12) ni les autres fonctions. Le code ne compilera peut-être pas encore complètement — c'est normal.

Pour tester demain après avoir fait create+yield : `make && ./test_yield`

---

➡️ Demain : `day11/` — TP03 : mthread_yield + mthread_create (TODO 8-14)
