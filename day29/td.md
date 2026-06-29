# Bilan Phases 1 à 3

*Pas de corrigé — c'est ta compréhension que tu consignes.*

---

## 1. Inventaire de ce que tu as implémenté

Liste toutes les fonctions que tu as écrites (ou commencé) :

```
mthread.c :
  □ mthread_create()
  □ mthread_yield()
  □ mthread_exit()
  □ mthread_join()
  □ thread_entry() (wrapper)
  □ scheduler_run()
  □ enqueue() / dequeue()

mthread_mutex.c :
  □ mthread_mutex_lock()
  □ mthread_mutex_unlock()
  □ mthread_mutex_trylock()

mthread_sem.c :
  □ mthread_sem_wait()
  □ mthread_sem_post()

mthread_cond.c :
  □ mthread_cond_wait()
  □ mthread_cond_signal()
  □ mthread_cond_broadcast()

preempt.c :
  □ sigalrm_handler()
  □ mthread_preempt_enable()
  □ mthread_preempt_disable()
```

---

## 2. Questions de réflexion

**Q1.** Quelle est la fonction la plus difficile à implémenter ? Pourquoi ?

```
Réponse :
```

**Q2.** Pourquoi `mthread_exit()` utilise `setcontext()` et pas `swapcontext()` ?

```
Réponse :
```

**Q3.** Si deux threads appellent `mthread_mutex_lock()` "en même temps", est-ce possible dans notre modèle N:1 ? Pourquoi ?

```
Réponse :
```

**Q4.** Que se passe-t-il si un thread appelle `malloc()` pendant que SIGALRM arrive et interrompt un autre thread qui était aussi dans `malloc()` ? (Indice : `malloc` n'est pas réentrant.)

```
Réponse :
```

**Q5.** Comment vérifier qu'il n'y a aucune fuite mémoire dans ta bibliothèque ?
(Donne la commande exacte.)

```
Réponse :
```

**Q6.** Quelle est la différence architecturale fondamentale entre `mthread` (N:1) et `pthread` (1:1) qui empêche `mthread` d'utiliser plusieurs cœurs CPU ?

```
Réponse :
```

---

## 3. Ce que tu vas faire dans la phase Projet

Phase 4 (jours 30-45) :
- Consolider tout le code en une bibliothèque propre `libmthread.a`
- Écrire des tests de non-régression
- Implémenter les fonctions manquantes (`trylock`, `timedwait`, `trywait`)
- Écrire une application démo
- Benchmarker mthread vs pthread
