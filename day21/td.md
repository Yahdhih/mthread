# TD — Exercices 1, 2, 3

---

## Exercice 1 — Bug `if` vs `while`

```c
static int ready = 0;
static mthread_mutex_t mu = MTHREAD_MUTEX_INITIALIZER;
static mthread_cond_t  cv = MTHREAD_COND_INITIALIZER;

void *waiter(void *arg) {
    mthread_mutex_lock(&mu);
    if (ready == 0)                      // ← BUG
        mthread_cond_wait(&cv, &mu);
    // On suppose ready == 1 ici
    printf("waiter : ready=%d\n", ready);
    mthread_mutex_unlock(&mu);
    return NULL;
}

void *signaler(void *arg) {
    // Simule un spurious wakeup : signal sans changer ready
    mthread_mutex_lock(&mu);
    mthread_cond_signal(&cv);            // signal sans ready=1 !
    mthread_mutex_unlock(&mu);

    mthread_yield();

    mthread_mutex_lock(&mu);
    ready = 1;
    mthread_cond_signal(&cv);
    mthread_mutex_unlock(&mu);
    return NULL;
}
```

**a)** Trace l'exécution. Qu'affiche `waiter` ? Quelle valeur a `ready` au moment où il lit ?

**b)** Corrige le code en changeant `if` en `while`. Retrace l'exécution.

**c)** Donne un exemple réel où `broadcast` nécessite absolument `while` : un pool de threads en attente d'un item dans un buffer de taille 1, avec plusieurs consommateurs. Que se passe-t-il avec `if` ?

---

**Espace de réponse :**
```
a)

b) void *waiter(void *arg) {
       mthread_mutex_lock(&mu);
       while (ready == 0)    // ← corrigé
           mthread_cond_wait(&cv, &mu);
       printf("waiter : ready=%d\n", ready);
       mthread_mutex_unlock(&mu);
       return NULL;
   }
   Trace :

c)
```

---

## Exercice 2 — Barrière avec Variable de Condition

Implémente une barrière pour N threads en utilisant mutex + cond.

```c
typedef struct {
    int count;
    int N;
    mthread_mutex_t mu;
    mthread_cond_t  cv;
} barrier_t;

void barrier_init(barrier_t *b, int N) {
    b->count = 0;
    b->N     = N;
    mthread_mutex_init(&b->mu);
    mthread_cond_init(&b->cv);
}

void barrier_wait(barrier_t *b) {
    mthread_mutex_lock(&b->mu);
    /* TODO : incrémenter count */
    /* TODO : si count < N, attendre (while !) */
    /* TODO : si count == N, broadcaster pour réveiller les autres */
    mthread_mutex_unlock(&b->mu);
}
```

**a)** Complète `barrier_wait`. Pourquoi utiliser `broadcast` plutôt que `signal` ?

**b)** Que se passe-t-il si on utilise `signal` à la place de `broadcast` ?

**c)** Cette barrière est-elle réutilisable (peut-on appeler barrier_wait une 2ème fois) ? Pourquoi ?

---

**Espace de réponse :**
```
a) void barrier_wait(barrier_t *b) {
       mthread_mutex_lock(&b->mu);
       b->count++;
       // compléter ici :

       mthread_mutex_unlock(&b->mu);
   }
   Raison du broadcast :

b)

c)
```

---

## Exercice 3 — Trace Producteur/Consommateur avec Condition

```c
int buffer[1];   // buffer taille 1
int count = 0;
mthread_mutex_t mu = MTHREAD_MUTEX_INITIALIZER;
mthread_cond_t  not_empty = MTHREAD_COND_INITIALIZER;
mthread_cond_t  not_full  = MTHREAD_COND_INITIALIZER;

// Producteur produit items 10, 20
// Consommateur consomme 2 items
```

Trace l'exécution entrelacée (en commençant par le consommateur) :

```
Action              | count | not_empty waiters | not_full waiters
--------------------|-------|-------------------|------------------
C: lock(mu)         |   0   |        []         |       []
C: while count==0 → |       |                   |
C: cond_wait(ne,mu) |       |                   |
  → unlock(mu)      |       |                   |
P: lock(mu)         |       |                   |
P: while count==1 → |       |                   |
P: buffer[0]=10     |       |                   |
P: count++          |       |                   |
P: signal(not_empty)|       |                   |
P: unlock(mu)       |       |                   |
C: réveillé, lock   |       |                   |
C: count==0 ? non   |       |                   |
C: item=buffer[0]   |       |                   |
C: count--          |       |                   |
C: signal(not_full) |       |                   |
C: unlock(mu)       |       |                   |
```

---

## Corrigés

<details>
<summary>Exercice 1</summary>

a) waiter appelle `cond_wait` (ready==0) → bloqué. signaler fait `signal` (sans changer ready) → waiter est réveillé. Avec `if`, waiter sort de l'attente sans vérifier → `ready=0`. Il affiche "waiter : ready=0". Bug !

b) Avec `while` : après le réveil du spurious signal, waiter revérifie `ready==0` → vrai → se rebloque. Quand signaler met `ready=1` et signal à nouveau → waiter se réveille, vérifie `ready==0` → faux → continue. Affiche "waiter : ready=1". Correct.

c) Avec broadcast sur buffer taille 1 et 3 consommateurs : tous se réveillent, mais 1 seul consomme. Les 2 autres avec `if` avancent sans item → erreur. Avec `while` : les 2 autres revérifient count==0 → vrai → se reblocent.

</details>

<details>
<summary>Exercice 2</summary>

a)
```c
void barrier_wait(barrier_t *b) {
    mthread_mutex_lock(&b->mu);
    b->count++;
    while (b->count < b->N)
        mthread_cond_wait(&b->cv, &b->mu);
    mthread_cond_broadcast(&b->cv);  // réveiller TOUS
    mthread_mutex_unlock(&b->mu);
}
```
Broadcast car N-1 threads attendent et tous doivent être réveillés.

b) Avec signal : un seul thread se réveille, il sort, les autres restent bloqués indéfiniment.

c) Non réutilisable : count reste à N. À la 2ème utilisation, tous les threads passent directement (count==N dès le début). Solution : remettre count=0 avant broadcast, ou utiliser une génération (count % 2).

</details>
