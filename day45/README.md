# Jour 45 — Bilan Final
**Durée : 1h** | Phase 4 — Dernier jour

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Vérifications finales | `checklist.sh` | 20 min |
| Remplir le bilan personnel | `BILAN.md` | 20 min |
| Lire les perspectives | `README.md` (ce fichier) | 20 min |

## Vérifications finales
```bash
# Lancer depuis ton répertoire projet/
make clean && make lib && make test
make valgrind

# Zero warning
make lib 2>&1 | grep -i warning | wc -l  # doit afficher 0

# Toutes les fonctions compilées
nm libmthread.a | grep ' T ' | sort
```

---

## Ce que tu as construit en 45 jours

```
Semaine 1-2 :
  Processus / Threads → Contexte d'exécution → ucontext_t
  → mthread_create / yield / exit / join

Semaine 3-4 :
  Mutex (exclusion mutuelle) → Sémaphores → Variables de condition
  → mthread_mutex / sem / cond

Semaine 5 :
  Scheduling FIFO → Round-Robin → Préemption (SIGALRM)
  → mthread_preempt_enable / disable

Semaines 6-7 :
  Bibliothèque complète → Tests → Benchmark → Application démo
  → libmthread.a
```

C'est exactement ce que font les ingénieurs systèmes qui écrivent `glibc` ou `musl`. Tu peux maintenant lire leur code source et le comprendre.

---

## Prochaines étapes

**Si tu veux aller plus loin :**

1. **Lire musl libc** (implémentation réelle de pthread) :
   `git clone https://git.musl-libc.org/cgit/musl`
   → `src/thread/` contient le vrai pthread

2. **Implémenter un modèle M:N** (N threads user → M threads noyau) :
   Ajouter `pthread_t` sous le capot de mthread pour avoir du vrai parallélisme

3. **Étudier le CFS de Linux** (Completely Fair Scheduler) :
   → `kernel/sched/fair.c` dans les sources du kernel

4. **Cours HPC avancé** :
   → OpenMP (directives de parallélisme)
   → MPI (parallélisme distribué)
   → CUDA (parallélisme GPU)

---

Remplis `BILAN.md` maintenant — dans 6 mois tu seras content d'avoir noté tes impressions à chaud.
