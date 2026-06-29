# Jour 28 — TP07 : préemption avec SIGALRM
**Durée : 1h** | Phase 3 — Scheduling

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Lire preempt.c (comprendre l'architecture) | `preempt.c` | 10 min |
| Implémenter TODO 1 (handler) | `preempt.c` | 10 min |
| Implémenter TODO 2-4 (enable/disable) | `preempt.c` | 15 min |
| Ajouter sigprocmask dans mthread_yield | `mthread.c` | 15 min |
| Tester | `make && ./test_preemptif` | 10 min |

## Ordre d'implémentation recommandé
1. Écrire `mthread_preempt_enable()` avec sigaction + setitimer
2. Écrire le handler (juste appeler `mthread_yield()`)
3. Ajouter `sigprocmask(SIG_BLOCK, ...)` au début de `mthread_yield()` et `SIG_UNBLOCK` à la fin
4. Tester avec `./test_preemptif` : les 2 threads doivent progresser

## Critère de réussite
Deux threads CPU-bound (sans yield explicite) affichent des compteurs **tous les deux** croissants.
Sans préemption : seul T0 s'exécute.

---
➡️ Demain : `day29/` — Validation + bilan phases 1-3
