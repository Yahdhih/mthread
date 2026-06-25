# TP 02 — Contexte d'exécution avec `ucontext`

> Objectif : maîtriser `getcontext`, `makecontext`, `swapcontext` — le fondement de mthread.
> Durée estimée : 2h.

---

## Exercice 1 — Ping-Pong entre deux contextes

Complète `pingpong.c` pour que deux fonctions s'alternent 5 fois chacune.

Sortie attendue :
```
PING 1
PONG 1
PING 2
PONG 2
PING 3
PONG 3
PING 4
PONG 4
PING 5
PONG 5
Fin.
```

---

## Exercice 2 — Passage d'argument

`makecontext` ne supporte que des arguments de type `int`. Pour passer un pointeur (64 bits), il faut le découper en deux `int`.

Complète `context_arg.c` qui passe une chaîne de caractères à une fonction de contexte.

---

## Exercice 3 — Chaîne de contextes

Crée 3 contextes C1, C2, C3. Chacun exécute une fonction et passe au suivant via `uc_link` (sans `swapcontext` explicite).

Sortie attendue :
```
C1 s'exécute
C2 s'exécute
C3 s'exécute
Retour dans main
```

---

## Exercice 4 — Détection de stack overflow

Dans `stack_test.c`, crée un contexte avec une pile très petite (512 octets) et une fonction récursive profonde. Observe ce qui se passe. Utilise `mprotect` pour créer une "guard page" à la fin de la pile.

---

## Comment tester

```bash
make
./pingpong          # doit afficher PING/PONG alternés
./context_arg       # doit afficher la chaîne passée
./chain             # doit afficher C1 C2 C3 Retour
```
