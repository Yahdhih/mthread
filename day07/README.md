# Jour 07 — TP02 : chaîne de contextes + révision
**Durée : 1h** | Phase 1 — Fondations ucontext

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| TP02 — Exercice 3 : chaîne circulaire de N contextes | `chain.c` | 45 min |
| Révision rapide : relis `day03/cours.md` sections ucontext | — | 15 min |

## Note du prof

La difficulté de cet exercice est la **circularité** : le thread N-1 doit passer au thread 0, pas à un thread inexistant. Utilise l'opération modulo : `(id + 1) % N`.

Le compteur `counter` est partagé entre tous les contextes. Comme on est en coopératif (un seul thread noyau), il n'y a pas de race condition — les accès sont séquentiels. C'est l'un des avantages du modèle M:1 coopératif.

Commencer par vérifier que ça compile sans les TODO, puis implémenter un TODO à la fois.

---

➡️ Demain : `day08/` — Cours 03 : cycle de vie d'un thread
