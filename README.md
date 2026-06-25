# mthread — Cours de Programmation Concurrente

> Objectif : comprendre et implémenter une bibliothèque de gestion de threads de zéro.

---

## Plan du cours

| #  | Cours                              | TD  | TP  | Statut |
|----|------------------------------------|-----|-----|--------|
| 01 | Processus et Threads — Fondements  | ✓   | ✓   | [ ]    |
| 02 | Contexte d'exécution               | ✓   | ✓   | [ ]    |
| 03 | Cycle de vie d'un thread           | ✓   | ✓   | [ ]    |
| 04 | Mutex et exclusion mutuelle        | ✓   | ✓   | [ ]    |
| 05 | Sémaphores                         | ✓   | ✓   | [ ]    |
| 06 | Variables de condition             | ✓   | ✓   | [ ]    |
| 07 | Scheduling                         | ✓   | ✓   | [ ]    |
| P  | Projet final — librairie mthread   | —   | —   | [ ]    |

---

## Structure du dossier

```
mthread/
├── README.md              ← tu es ici
├── cours/                 ← cours magistraux (définitions, théorèmes)
├── td/td_0X/              ← exercices théoriques (papier/crayon)
├── tp/tp_0X/              ← exercices pratiques (code C)
└── projet/                ← projet final intégrateur
```

---

## Prérequis

- C (pointeurs, malloc, structs)
- Ligne de commande Linux/macOS
- `gcc`, `make`, `valgrind`

## Ordre de travail recommandé

Pour chaque chapitre :
1. Lire le cours dans `cours/`
2. Faire le TD (questions théoriques, réponses sur papier)
3. Faire le TP (code dans `tp/tp_0X/`)
4. Valider avec `make test` dans le dossier TP

---

## Compilateur et flags recommandés

```bash
gcc -Wall -Wextra -g -fsanitize=thread,address -o prog prog.c -lpthread
```
