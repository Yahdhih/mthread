# Jour 04 — TD02 : swapcontext en détail
**Durée : 1h** | Phase 1 — Fondations ucontext

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Exercices 2, 3, 4 sur ucontext_t | `td.md` | 1h |

## Note du prof

Ces exercices sont essentiels. `getcontext`, `makecontext`, et `swapcontext` sont les briques de base de toute la bibliothèque mthread que tu vas construire. Ne passe pas à day05 avant d'avoir compris Q3 (le ping-pong pas à pas) — c'est le cœur du mécanisme.

**À ne pas confondre :**
- `getcontext(ctx)` : capture l'état actuel dans `ctx` (comme une photo)
- `makecontext(ctx, func, n, ...)` : modifie `ctx` pour qu'il pointe vers `func` (nécessite que `uc_stack` soit configurée avant)
- `swapcontext(old, new)` : sauvegarde l'état courant dans `old`, restaure `new`

---

➡️ Demain : `day05/` — TD02 fin + TP02 début : pingpong
