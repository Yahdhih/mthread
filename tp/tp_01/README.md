# TP 01 — Observer les threads avec les outils système

> Objectif : comprendre les threads via les outils Unix avant d'en coder.
> Durée estimée : 1h.

---

## Partie 1 — `ps` et `top`

Compile et lance `observer.c` :

```bash
gcc -o observer observer.c -lpthread && ./observer &
ps -eLf | grep observer    # -L = afficher les threads (LWP)
```

**Questions :**
1. Combien de LWP (Light Weight Process = threads) vois-tu ?
2. Quel est le PID du processus ? Les LWP partagent-ils le même PID ?
3. Lance `top -H` (mode threads). Trouve les threads de `observer`.

---

## Partie 2 — `/proc`

```bash
ls /proc/$(pgrep observer)/task/    # liste les threads du processus
cat /proc/$(pgrep observer)/status  # infos sur les threads
```

**Questions :**
1. Que contient le répertoire `task/` ?
2. Qu'est-ce que `Threads:` dans `/proc/.../status` ?

---

## Partie 3 — `valgrind`

```bash
gcc -g -o observer observer.c -lpthread
valgrind --tool=helgrind ./observer_race   # détecter les races
valgrind --leak-check=full ./observer      # détecter les leaks
```

**Questions :**
1. `observer_race.c` contient une race condition volontaire. Helgrind la détecte-t-il ?
2. Que rapporte `--leak-check=full` si on oublie un `join` ?

---
