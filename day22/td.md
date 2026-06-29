# TD — Exercices 4 et 5

---

## Exercice 4 — File d'Attente Thread-Safe

Conçois une file FIFO thread-safe : `push` est non-bloquant (si pleine → erreur), `pop` est **bloquant** (attend si vide).

**a)** Complète la structure et l'implémentation :

```c
#define QUEUE_MAX 8

typedef struct {
    int  data[QUEUE_MAX];
    int  head, tail, count;
    mthread_mutex_t mu;
    mthread_cond_t  not_empty;
} safe_queue_t;

void queue_init(safe_queue_t *q) {
    q->head = q->tail = q->count = 0;
    mthread_mutex_init(&q->mu);
    mthread_cond_init(&q->not_empty);
}

int queue_push(safe_queue_t *q, int val) {
    mthread_mutex_lock(&q->mu);
    /* TODO : si count == QUEUE_MAX → return -1 (pleine) */
    /* TODO : insérer val et incrémenter tail et count */
    /* TODO : signaler not_empty */
    mthread_mutex_unlock(&q->mu);
    return 0;
}

int queue_pop(safe_queue_t *q) {
    mthread_mutex_lock(&q->mu);
    /* TODO : while count == 0 → attendre not_empty */
    /* TODO : retirer et retourner l'élément */
    mthread_mutex_unlock(&q->mu);
    return -1; /* placeholder */
}
```

**b)** Peut-on avoir plusieurs threads appelant `queue_pop` simultanément ? Que se passe-t-il si 2 threads attendent et qu'un seul item est poussé, puis `broadcast` est utilisé à la place de `signal` ?

**c)** Propose une version avec timeout : `queue_pop_timeout(q, ms)` retourne -1 si rien n'arrive dans `ms` millisecondes. (Pseudocode uniquement — `mthread_cond_timedwait` n'est pas encore implémenté.)

---

**Espace de réponse :**
```
a) queue_push :
   queue_pop :

b)

c) int queue_pop_timeout(safe_queue_t *q, int ms) {
       ...
   }
```

---

## Exercice 5 — Inversion de Priorité

**Scénario** : 3 threads avec priorités (3 = haute, 1 = basse) :

```
T_bas   (prio 1) : fait lock(M), calcul long, unlock(M)
T_haut  (prio 3) : fait lock(M) ← attend M
T_moyen (prio 2) : calcul CPU (ne touche pas M)
```

**Timeline** (avec scheduler à priorités) :
```
t=0  T_bas démarre, lock(M) → M verrouillé
t=1  T_haut arrive, veut lock(M) → bloqué (M pris par T_bas)
t=2  T_moyen arrive
t=3  Scheduler : T_moyen a prio 2 > T_bas prio 1 → T_moyen préempte T_bas !
t=4  T_moyen calcule...
t=5  T_moyen finit
t=6  T_bas reprend, finit calcul, unlock(M)
t=7  T_haut peut enfin lock(M)
```

**a)** Combien de temps T_haut (priorité 3) attend-il M ? Qui le retarde réellement ?

**b)** Identifie le problème : T_haut de priorité 3 est retardé par T_moyen de priorité 2 qui n'a aucun lien avec M. Comment s'appelle ce phénomène ?

**c)** Solution par **héritage de priorité** : quand T_haut attend M (détenu par T_bas), T_bas reçoit temporairement la priorité de T_haut. Retrace la timeline avec cette solution.

**d)** Ce problème a causé un vrai bug célèbre. Nomme-le (indice : Mars, 1997).

---

**Espace de réponse :**
```
a)

b)

c) Timeline avec héritage :
t=0  T_bas lock(M)
t=1  T_haut veut M → bloqué. T_bas reçoit prio=3 (héritage)
t=2  T_moyen arrive (prio=2)
t=3  Scheduler : T_bas a prio=3 > T_moyen prio=2 → T_bas continue !
...

d)
```

---

## Corrigés

<details>
<summary>Exercice 4</summary>

a)
```c
int queue_push(safe_queue_t *q, int val) {
    mthread_mutex_lock(&q->mu);
    if (q->count == QUEUE_MAX) { mthread_mutex_unlock(&q->mu); return -1; }
    q->data[q->tail % QUEUE_MAX] = val;
    q->tail++;
    q->count++;
    mthread_cond_signal(&q->not_empty);
    mthread_mutex_unlock(&q->mu);
    return 0;
}

int queue_pop(safe_queue_t *q) {
    mthread_mutex_lock(&q->mu);
    while (q->count == 0)
        mthread_cond_wait(&q->not_empty, &q->mu);
    int val = q->data[q->head % QUEUE_MAX];
    q->head++;
    q->count--;
    mthread_mutex_unlock(&q->mu);
    return val;
}
```

b) Oui, plusieurs threads peuvent appeler `queue_pop`. Avec `broadcast` et 1 item : les 2 threads se réveillent, mais le `while` protège : un seul passe (count passe à 0), l'autre voit count==0 et se rebloque.

c)
```c
int queue_pop_timeout(safe_queue_t *q, int ms) {
    mthread_mutex_lock(&q->mu);
    struct timespec deadline = now() + ms;
    while (q->count == 0) {
        if (mthread_cond_timedwait(&q->not_empty, &q->mu, ms) == ETIMEDOUT) {
            mthread_mutex_unlock(&q->mu);
            return -1;
        }
    }
    int val = q->data[q->head++ % QUEUE_MAX];
    q->count--;
    mthread_mutex_unlock(&q->mu);
    return val;
}
```

</details>

<details>
<summary>Exercice 5</summary>

a) T_haut attend de t=1 à t=7 : 6 unités. T_moyen (prio 2) le retarde alors qu'il n'a rien à voir avec M.

b) C'est l'**inversion de priorité** : une tâche de haute priorité est effectivement bloquée par une tâche de priorité inférieure, via une troisième tâche qui préempte la tâche à basse priorité qui détient la ressource.

c) Timeline avec héritage :
```
t=0 T_bas lock(M)
t=1 T_haut bloqué → T_bas.prio = 3 (héritage)
t=2 T_moyen arrive (prio 2)
t=3 Scheduler : T_bas prio=3 > T_moyen prio=2 → T_bas continue
t=4 T_bas unlock(M) → T_bas.prio revient à 1
t=5 T_haut peut lock(M) → T_haut s'exécute (prio 3)
```
T_haut attend seulement de t=1 à t=5 : 4 unités. Plus équitable.

d) **Mars Pathfinder (1997)** : le VxWorks RTOS subissait une inversion de priorité entre la tâche de communication (haute prio), une tâche d'acquisition météo (basse prio détenant un mutex), et une tâche intermédiaire. Le watchdog redémarrait le système. Résolu par activation de l'héritage de priorité dans VxWorks.

</details>
