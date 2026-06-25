# Cours 03 — Cycle de vie d'un thread

---

## 1. Les états d'un thread

### Définition 1.1 — Machine à états d'un thread
Un thread passe par plusieurs **états** au cours de sa vie :

```
                  create()
                     │
                     ▼
               ┌──────────┐
         ┌────►│  READY   │◄────────────────────┐
         │     └──────────┘                     │
         │           │ scheduler choisit         │
         │           ▼                           │
         │     ┌──────────┐    yield() /         │
  unlock │     │ RUNNING  │────mutex busy────►┌──────────┐
  signal │     └──────────┘                   │ BLOCKED  │
         │           │ return / exit()         └──────────┘
         │           ▼
         │     ┌──────────┐
         └─────│  ZOMBIE  │◄── thread terminé, attend join()
               └──────────┘
                     │ join()
                     ▼
                  [libéré]
```

### Définition 1.2 — État READY (Prêt)
Le thread est **prêt à s'exécuter** mais pas encore sélectionné par le scheduler. Il est dans la **file d'attente** des threads prêts (run queue).

### Définition 1.3 — État RUNNING (En cours)
Le thread **occupe le CPU**. Un seul thread peut être RUNNING par cœur. Il passe à READY via `yield()` ou à BLOCKED en attendant une ressource.

### Définition 1.4 — État BLOCKED (Bloqué)
Le thread **attend une ressource** (mutex verrouillé, sémaphore à 0, I/O...). Il n'est pas dans la run queue : le scheduler ne le sélectionnera pas tant que la ressource n'est pas disponible.

### Définition 1.5 — État ZOMBIE
Le thread a **terminé** (sa fonction `return` a été exécutée) mais ses ressources n'ont pas encore été libérées. Il attend qu'un autre thread appelle `join()` pour récupérer sa valeur de retour.

---

## 2. Création d'un thread

### Définition 2.1 — `mthread_create`
```c
int mthread_create(mthread_t *tid, void *(*func)(void *), void *arg);
```
- Alloue une structure thread avec sa pile
- Initialise le contexte pour exécuter `func(arg)`
- Place le thread en état **READY**
- Retourne un identifiant `tid`

### Séquence d'initialisation
```
mthread_create() :
  1. malloc(sizeof(mthread_t))       ← allouer la structure
  2. malloc(STACK_SIZE)              ← allouer la pile
  3. getcontext(&t->ctx)             ← initialiser le contexte
  4. t->ctx.uc_stack.ss_sp = stack   ← pointer vers la pile
  5. t->ctx.uc_stack.ss_size = SIZE  ← taille
  6. t->ctx.uc_link = &scheduler_ctx ← où aller quand func() retourne
  7. makecontext(&t->ctx, func, 1, arg) ← configurer le point d'entrée
  8. t->state = READY                ← ajouter à la run queue
```

**Point clé — `uc_link`** : quand `func()` retourne, le contexte `uc_link` est automatiquement activé. On y met le contexte du scheduler pour reprendre la main.

---

## 3. Yield — Céder le CPU volontairement

### Définition 3.1 — `mthread_yield`
```c
void mthread_yield(void);
```
Le thread courant **cède volontairement** le CPU. Il passe de RUNNING à READY et est replacé en fin de run queue. Le scheduler choisit le prochain thread.

```
T1 (RUNNING) appelle yield() :
  1. T1 → READY (ajouté en fin de file)
  2. Scheduler choisit T2 (premier de la file)
  3. T2 → RUNNING
  4. swapcontext(&T1->ctx, &scheduler_ctx)
```

### Théorème 3.1 — Scheduling coopératif
Dans un modèle **coopératif**, un thread ne rend le CPU que s'il appelle `yield()`, `join()`, ou attend une ressource. Un thread qui ne coopère pas peut monopoliser le CPU indéfiniment.

---

## 4. Join — Attendre la fin d'un thread

### Définition 4.1 — `mthread_join`
```c
int mthread_join(mthread_t tid, void **retval);
```
- Bloque le thread appelant jusqu'à ce que `tid` se termine
- Récupère la valeur de retour dans `*retval`
- Libère les ressources du thread `tid`

### Théorème 4.1 — Nécessité du join
Si un thread se termine sans `join()`, ses ressources ne sont **jamais libérées** : c'est une **fuite de ressources** (équivalent d'un memory leak pour les threads).

**Exception** : un thread peut être détaché (`detach`) pour se libérer automatiquement, mais il ne peut plus être joint.

### Cas d'usage du join
```c
// Pattern classique
mthread_t t;
mthread_create(&t, worker, arg);
// ... le thread principal continue ...
void *result;
mthread_join(t, &result);   // attend que worker() finisse
printf("Résultat : %p\n", result);
```

---

## 5. Exit — Terminer un thread

### Définition 5.1 — `mthread_exit`
```c
void mthread_exit(void *retval);
```
Termine le thread courant avec la valeur de retour `retval`. Équivalent d'un `return retval` dans la fonction du thread, mais peut être appelé de n'importe où.

---

## 6. Structure de données d'un thread

```c
typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    ZOMBIE
} mthread_state_t;

typedef struct mthread {
    int              id;           // identifiant unique
    ucontext_t       ctx;          // contexte d'exécution (registres + pile)
    char            *stack;        // pile allouée
    size_t           stack_size;   // taille de la pile
    mthread_state_t  state;        // état courant
    void            *retval;       // valeur de retour (quand ZOMBIE)
    struct mthread  *next;         // maillon pour les listes chaînées
} mthread_t;
```

---

## 7. La run queue

### Définition 7.1 — Run queue
La **run queue** est la liste des threads en état READY, attendant d'être sélectionnés par le scheduler.

```
Run queue (liste chaînée circulaire) :
  ┌──────┐    ┌──────┐    ┌──────┐
  │  T2  │───►│  T4  │───►│  T5  │───► (tête)
  └──────┘    └──────┘    └──────┘
  READY       READY        READY
```

Le scheduler prend toujours le **premier** thread de la file (FIFO simple).

---

## Résumé du cours 03

| Concept            | À retenir                                              |
|--------------------|--------------------------------------------------------|
| READY              | Prêt, dans la run queue                                |
| RUNNING            | Occupe le CPU                                          |
| BLOCKED            | Attend une ressource, pas dans la run queue            |
| ZOMBIE             | Terminé, attend join()                                 |
| `yield()`          | Cède le CPU volontairement                             |
| `join()`           | Attend un thread et libère ses ressources              |
| `uc_link`          | Contexte à activer quand la fonction du thread retourne|

---

**Précédent** : [Cours 02](cours_02_contexte_execution.md) | **Suivant** : [Cours 04 — Mutex](cours_04_mutex.md)
