# Jour 20 — TP05 validation + début Cours 06
**Durée : 1h** | Phase 2 — Synchronisation

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Finaliser et valider TP05 | `mthread_sem.c` | 30 min |
| Lire le cours (sections 1-3) | `cours.md` | 30 min |

## Tests à faire passer
```bash
make && ./test_prod_conso          # doit afficher [PASS]
valgrind --leak-check=full ./test_prod_conso  # 0 errors
```

## À ne pas rater dans le cours
Section 3 : l'atomicité de cond_wait — lis-la **deux fois**. C'est le point le plus subtil de toute la bibliothèque.

---
➡️ Demain : `day21/` — Cours 06 fin + TD06 exercices 1-3
