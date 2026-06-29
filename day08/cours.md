# Cours 03 — Cycle de vie d'un thread

---

## 1. Les 4 états d'un thread

Un thread dans notre bibliothèque mthread peut être dans l'un des 4 états suivants :

```
                    ┌─────────────────────────────────────┐
                    │                                     │
         mthread_create()                          mthread_yield()
                    │                                     │
                    ▼                                     │
              ┌──────────┐    scheduler             ┌──────────┐
              │  READY   │ ──────────────────────► │ RUNNING  │
              └──────────┘                          └──────────┘
                    ▲                                │        │
                    │                        mthread_exit()   │ mthread_join()
          mthread_exit() du joiner           (sur cible)      │ (sur thread bloquant)
          ou mutex_unlock()                         │         │
                    │                               ▼         ▼
                    │                         ┌──────────┐ ┌──────────┐
                    └─────────────────────────│  ZOMBIE  │ │ BLOCKED  │
                                              └──────────┘ └──────────┘
```

### Définition 1 : État READY
Un thread est en état **READY** (prêt) s'il est éligible à être exécuté par le scheduler, mais qu'il n'est pas actuellement en cours d'exécution. Il attend dans la file des threads prêts.

### Définition 2 : État RUNNING
Un thread est en état **RUNNING** (en cours d'exécution) s'il possède le CPU. Dans un modèle M:1 (N threads utilisateur sur 1 thread noyau), **un seul thread peut être RUNNING à un instant donné**.

### Définition 3 : État BLOCKED
Un thread est en état **BLOCKED** (bloqué) s'il attend qu'une condition externe se réalise (mutex disponible, thread-cible terminé pour join, sémaphore > 0...). Un thread BLOCKED n'est pas dans la file READY — il ne consomme pas de CPU.

### Définition 4 : État ZOMBIE
Un thread est en état **ZOMBIE** s'il a terminé son exécution (via `mthread_exit`) mais que sa valeur de retour n'a pas encore été récupérée par un `mthread_join`. La structure du thread existe encore en mémoire pour stocker `retval`.

---

## 2. Les transitions entre états

```
Transition              Condition                 Action
──────────────────────────────────────────────────────────────────
NONEXISTANT → READY     mthread_create()          allouer struct, configurer contexte,
                                                  enqueue dans ready_queue
READY → RUNNING         scheduler le choisit      swapcontext(scheduler, thread)
RUNNING → READY         mthread_yield()           enqueue(current), swapcontext(current, scheduler)
RUNNING → BLOCKED       mthread_join() sur thread  current->state=BLOCKED,
                        non-terminé, ou           thread->joiner=current,
                        mutex_lock() si locké     swapcontext(current, scheduler)
RUNNING → ZOMBIE        mthread_exit()            current->state=ZOMBIE,
                                                  current->retval=val,
                                                  réveiller joiner si besoin,
                                                  setcontext(scheduler)
BLOCKED → READY         mthread_exit() de la      enqueue(joiner)
                        cible, ou mutex_unlock()
ZOMBIE → NONEXISTANT    mthread_join() récupère   free(struct), free(stack)
                        retval
```

---

## 3. La file des threads READY (liste chaînée)

La file READY est une **liste chaînée FIFO** (First In, First Out). Les nouveaux threads (ou les threads qui yielden) s'ajoutent en queue (*enqueue*). Le scheduler prend en tête (*dequeue*).

```
ready_queue (tête)
       │
       ▼
   ┌───────┐    ┌───────┐    ┌───────┐
   │  T2   │───►│  T4   │───►│  T1   │──► NULL
   └───────┘    └───────┘    └───────┘
      READY        READY        READY
   (suivant à     (puis lui)   (en dernier)
    s'exécuter)
```

**Invariant de la file :**
- Tout thread en état READY est dans la file, et uniquement dans la file.
- Un thread en RUNNING, BLOCKED ou ZOMBIE n'est **pas** dans la file.

### Structure C de la file :

```c
static mthread_struct_t *ready_queue = NULL;  // tête de la file

// Ajouter en queue
void enqueue(mthread_struct_t *t) {
    t->next = NULL;
    if (ready_queue == NULL) {
        ready_queue = t;
        return;
    }
    mthread_struct_t *p = ready_queue;
    while (p->next != NULL)
        p = p->next;
    p->next = t;
}

// Retirer la tête
mthread_struct_t *dequeue(void) {
    if (ready_queue == NULL) return NULL;
    mthread_struct_t *t = ready_queue;
    ready_queue = t->next;
    t->next = NULL;
    return t;
}
```

---

## 4. Algorithme de mthread_create

`mthread_create` crée un nouveau thread et le place dans la file READY.

```
Algorithme mthread_create(thread_ptr, func, arg) :
  1. Allouer mthread_struct_t *t = calloc(...)
  2. t->id    = next_id++
  3. t->func  = func
  4. t->arg   = arg
  5. t->state = READY
  6. t->joiner = NULL
  7. t->retval = NULL
  8. t->next   = NULL
  9. Allouer t->stack = malloc(STACK_SIZE)
 10. getcontext(&t->ctx)
 11. t->ctx.uc_stack.ss_sp   = t->stack
     t->ctx.uc_stack.ss_size = STACK_SIZE
     t->ctx.uc_link          = &scheduler_ctx  (ou NULL si on gère avec setcontext)
 12. makecontext(&t->ctx, thread_entry, 0)
     (thread_entry est un wrapper interne qui appelle func(arg) puis mthread_exit)
 13. enqueue(t)
 14. Si c'est le premier thread créé : initialiser scheduler_ctx avec getcontext
     (le scheduler a besoin d'une pile ou peut utiliser la pile du main)
 15. *thread_ptr = t
 16. return 0
```

**Pourquoi un wrapper `thread_entry` ?**

```c
static void thread_entry(void) {
    void *ret = current->func(current->arg);
    mthread_exit(ret);
    // On n'atteint jamais cette ligne
}
```

Le wrapper garantit que même si la fonction du thread retourne sans appeler `mthread_exit`, la transition vers ZOMBIE se fait proprement.

---

## 5. Algorithme de mthread_yield

`mthread_yield` cède le CPU au scheduler.

```
Algorithme mthread_yield() :
  1. current->state = READY
  2. enqueue(current)
  3. swapcontext(&current->ctx, &scheduler_ctx)
     ← le thread s'endort ici
     ← il reprendra après le swapcontext quand le scheduler le réveillera
```

**Diagramme :**

```
[Thread A — RUNNING]
     │
     │ appelle mthread_yield()
     │
     ▼
  A->state = READY
  enqueue(A)           ← A rejoint la queue
  swapcontext(A, sched) ← A s'endort, scheduler se réveille
     │
     ▼
[Scheduler]
  next = dequeue()    ← prend B (ou C, ou A lui-même s'il était seul)
  next->state = RUNNING
  current = next
  swapcontext(sched, next) ← scheduler s'endort, B se réveille
     │
     ▼
[Thread B — RUNNING]
  reprend juste après son propre swapcontext
```

---

## 6. Algorithme de mthread_exit

`mthread_exit` termine un thread et déclenche la transition vers ZOMBIE.

```
Algorithme mthread_exit(retval) :
  1. current->state  = ZOMBIE
  2. current->retval = retval
  3. Si current->joiner != NULL :
       joiner->state = READY
       enqueue(joiner)         ← réveiller le thread qui attend ce join
  4. setcontext(&scheduler_ctx)  ← sans retour possible (le thread est mort)
     (NB : setcontext, pas swapcontext — on ne sauve pas le contexte ZOMBIE)
```

**Important :** on utilise `setcontext` et non `swapcontext`. `swapcontext` sauvegarderait le contexte actuel (qui n'a plus de sens puisque le thread est ZOMBIE). `setcontext` charge simplement le contexte du scheduler sans sauvegarder quoi que ce soit.

---

## 7. Algorithme de mthread_join

`mthread_join` attend qu'un thread se termine et récupère sa valeur de retour.

```
Algorithme mthread_join(thread, retval_ptr) :
  Cas 1 : thread->state == ZOMBIE
    → La cible est déjà terminée
    → Si retval_ptr != NULL : *retval_ptr = thread->retval
    → Libérer la mémoire du thread (free stack, free struct)
    → return 0

  Cas 2 : thread->state != ZOMBIE
    → La cible tourne encore (READY ou RUNNING ou BLOCKED)
    → current->state = BLOCKED
    → thread->joiner = current      ← mémoriser qui attend
    → swapcontext(&current->ctx, &scheduler_ctx)
       ← on s'endort, le scheduler choisit un autre thread
       ← quand thread appellera mthread_exit, il nous réveillera
       ← on reprend ici
    → (maintenant thread->state == ZOMBIE)
    → Si retval_ptr != NULL : *retval_ptr = thread->retval
    → Libérer la mémoire du thread
    → return 0
```

**Attention :** un seul thread peut joindre un autre à la fois. Si deux threads essaient de joindre le même, le comportement est indéfini (on ne gère pas ce cas dans mthread).

---

## 8. Vue d'ensemble : le scheduler

```c
static void scheduler_run(void) {
    while (1) {
        mthread_struct_t *next = dequeue();
        if (next == NULL) {
            return;  // plus rien à exécuter, retour au main
        }
        next->state = RUNNING;
        current = next;
        swapcontext(&scheduler_ctx, &next->ctx);
        // ← on reprend ici après chaque yield/exit
    }
}
```

Le scheduler est une boucle infinie : défile, exécute, recommence. Il s'arrête quand la file est vide.

**Invariant du scheduler :**
- Quand le scheduler s'exécute, `current` peut être NULL ou pointer vers le thread qui vient de yield/exit.
- Après `swapcontext(&scheduler_ctx, &next->ctx)`, le scheduler reprendra exactement à cette ligne-là lors du prochain yield/exit.

---

## 9. Diagramme complet : scénario avec 2 threads

```
Timeline :
  main crée T1 et T2
  main appelle yield → scheduler démarre
  T1 s'exécute, yield
  T2 s'exécute, exit
  T1 s'exécute, exit
  scheduler : file vide, retour au main
  main join T1, join T2


  main          scheduler        T1            T2
   │                              │             │
   │ create(T1) ───────────────► [READY]       │
   │ create(T2) ────────────────────────────► [READY]
   │                                           │
   │ yield ──────────────────────────────────────────►
   │                │                          │
   │         dequeue→T1                        │
   │                │ swap(sched,T1)           │
   │                │─────────────────────────►│ [RUNNING]
   │                │                          │ yield
   │                │◄─────────────────────────│
   │                │                   [READY] enqueue(T1)
   │         dequeue→T2                        │
   │                │ swap(sched,T2)           │
   │                │──────────────────────────────────►[RUNNING]
   │                │                                   exit
   │                │◄──────────────────────────────────[ZOMBIE]
   │         T1 joiner? non
   │         dequeue→T1                        │
   │                │ swap(sched,T1)           │
   │                │─────────────────────────►│ [RUNNING]
   │                │                          │ exit
   │                │◄─────────────────────────│[ZOMBIE]
   │         dequeue→NULL (file vide)
   │                │ return
   │◄───────────────│
   │ join(T1) → ZOMBIE, retval OK
   │ join(T2) → ZOMBIE, retval OK
  fin
```

---

**Résumé clé :**

| Opération      | Qui s'endort | Qui se réveille |
|---------------|--------------|-----------------|
| `yield`       | thread courant | scheduler     |
| `exit`        | thread courant (définitivement) | scheduler (+ joiner si besoin) |
| `join` (bloquant) | thread appelant | scheduler |
| `scheduler`   | scheduler | prochain thread READY |
