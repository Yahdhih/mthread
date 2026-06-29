# Jour 23 — TP06 : implémenter mthread_cond
**Durée : 1h** | Phase 2 — Synchronisation

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Lire mthread_cond.h | comprendre la structure | 5 min |
| Implémenter cond_wait (TODO 1-4) | `mthread_cond.c` | 30 min |
| Implémenter cond_signal + broadcast | `mthread_cond.c` | 20 min |
| Compiler | `make` | 5 min |

## Ordre CRITIQUE dans cond_wait
1. Ajouter à la queue (AVANT de relâcher le mutex)
2. Relâcher le mutex
3. Appeler `_mthread_schedule()` (dormir)
4. Au réveil : réacquérir le mutex

Si tu inverses 1 et 2, tu perds des signaux. Le programme semble fonctionner parfois mais plante aléatoirement.

---
➡️ Demain : `day24/` — TP06 : test producteur/consommateur complet
