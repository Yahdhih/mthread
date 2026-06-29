# Jour 06 — TP02 : context_arg — passer des arguments
**Durée : 1h** | Phase 1 — Fondations ucontext

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| TP02 — Exercice 2 : passer des arguments à un contexte | `context_arg.c` | 1h |

## Note du prof

`makecontext` ne peut passer que des `int` directement en arguments. C'est une limitation de l'API POSIX. Pour passer un `void *` (pointeur), deux stratégies :

1. **Variable globale** : stocker l'argument dans une structure globale avant le swap, le lire depuis la fonction.
2. **Deux int** : caster le pointeur en `(uintptr_t)`, puis passer les 32 bits hauts et bas comme deux `int`. Reconstituer dans la fonction avec `(void *)((uintptr_t)hi << 32 | (uintptr_t)lo)`.

Pour cet exercice, utilise la stratégie la plus simple : passe directement un `int id` (l'indice).

Vérifie ta solution avec `make && ./context_arg`. La sortie attendue :
```
Thread 0 démarré
Thread 1 démarré
Thread 2 démarré
Tous les threads ont terminé
```

---

➡️ Demain : `day07/` — TP02 : chaîne de contextes + révision
