# Jour 05 — TD02 fin + TP02 début : pingpong
**Durée : 1h** | Phase 1 — Fondations ucontext

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Exercice 5 : jeton entre 3 contextes | `td.md` | 20 min |
| TP02 — Exercice 1 : implémenter ping-pong | `pingpong.c` | 40 min |

## Note du prof

Pour le TP, suis les TODO dans l'ordre. La partie la plus délicate est l'initialisation des contextes (`getcontext` → configurer `uc_stack` → `makecontext`). La pile doit être allouée **avant** d'appeler `makecontext`.

Pour tester : `make && ./pingpong`. La sortie attendue est dans le commentaire en tête de `pingpong.c`.

---

➡️ Demain : `day06/` — TP02 : context_arg — passer des arguments
