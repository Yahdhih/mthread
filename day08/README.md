# Jour 08 — Cours 03 : cycle de vie d'un thread
**Durée : 1h** | Phase 2 — Bibliothèque mthread

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Cours : cycle de vie, file READY, algorithmes | `cours.md` | 40 min |
| TD03 — Exercice 1 : tracer un scénario | `td.md` | 20 min |

## Note du prof

Ce cours est le pivot du projet. Tu vas passer des exercices ucontext isolés à une vraie bibliothèque de threads. Lis `cours.md` en entier avant de faire l'exercice.

Concentre-toi sur les algorithmes de `mthread_yield` et `mthread_exit` — c'est là que les gens se perdent. La clé : `swapcontext` vers le scheduler, pas directement vers le prochain thread.

---

➡️ Demain : `day09/` — TD03 : exercices 2 à 5
