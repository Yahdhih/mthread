# Cours — Sémaphores

---

## 1. Motivation — Pourquoi le mutex ne suffit pas

Le mutex résout l'**exclusion mutuelle** mais pas la **synchronisation conditionnelle**.

**Problème** : producteur qui attend qu'il y ait de la place dans un buffer.

Avec mutex seul → attente active (busy-wait) :
```c
while (1) {
    mthread_mutex_lock(&mu);
    if (count < MAX) break;        // attendre de la place
    mthread_mutex_unlock(&mu);
    mthread_yield();               // gaspille CPU pour vérifier encore
}
buffer[in++] = item;
mthread_mutex_unlock(&mu);
```
C'est inefficace. Le sémaphore résout cela élégamment.

---

## 2. Définition Formelle

### Définition 2.1 — Sémaphore (Dijkstra, 1965)
Un **sémaphore** S est un entier ≥ 0 avec deux opérations **atomiques** :

**`sem_wait(S)`** (aussi appelé P, down, acquire) :
```
si S > 0 :
    S--        ← décrémenter et continuer
sinon :
    bloquer le thread courant jusqu'à ce que S > 0
```

**`sem_post(S)`** (aussi appelé V, up, release) :
```
si des threads attendent :
    réveiller un thread en attente
    (S reste à 0 — le thread réveillé "prend" le jeton)
sinon :
    S++
```

---

## 3. Sémaphore Binaire vs Compteur

### Définition 3.1 — Sémaphore binaire
S ∈ {0, 1}. Équivalent à un mutex **sans ownership**.

### Définition 3.2 — Sémaphore compteur
S ∈ ℕ. Représente un nombre de ressources disponibles.

**Exemple** : 5 imprimantes disponibles.
```c
sem_t printers;
sem_init(&printers, 5);   // 5 disponibles

void utiliser_imprimante(void) {
    sem_wait(&printers);   // prendre une imprimante (S : 5→4→3...)
    imprimer();
    sem_post(&printers);   // relâcher (S : 3→4...)
}
```

---

## 4. Sémaphore vs Mutex

| Caractéristique | Mutex       | Sémaphore binaire |
|-----------------|-------------|-------------------|
| Valeur initiale | 1 (libre)   | 0 ou 1            |
| Ownership       | Oui — seul le détenteur peut unlock | Non — n'importe qui peut post |
| Usage principal | Section critique | Signalisation |
| Deadlock potentiel | Si mal ordonné | Si init mauvaise |

**Règle d'or** : utiliser mutex pour les sections critiques, sémaphore pour la synchronisation entre producteurs et consommateurs.

---

## 5. Producteur / Consommateur

**Problème** : 1 producteur, 1 consommateur, buffer circulaire de taille N.

**Solution avec 2 sémaphores** :
```c
sem_t sem_empty;  // nombre de places vides  (init = N)
sem_t sem_full;   // nombre d'éléments prêts (init = 0)

void *producteur(void *arg) {
    for (int i = 0; i < TOTAL; i++) {
        int item = produire();
        sem_wait(&sem_empty);  // attendre qu'il y ait de la place
        buffer[in++ % N] = item;
        sem_post(&sem_full);   // signaler qu'un élément est prêt
    }
    return NULL;
}

void *consommateur(void *arg) {
    for (int i = 0; i < TOTAL; i++) {
        sem_wait(&sem_full);   // attendre qu'il y ait un élément
        int item = buffer[out++ % N];
        sem_post(&sem_empty);  // signaler qu'une place est libre
        consommer(item);
    }
    return NULL;
}

int main(void) {
    sem_init(&sem_empty, N);  // N places vides au départ
    sem_init(&sem_full,  0);  // 0 éléments prêts au départ
    // créer et joindre les threads...
}
```

**Trace** (N=2, TOTAL=3) :
```
sem_empty=2, sem_full=0
P: sem_wait(empty)→1, buffer[0]=0, sem_post(full)→1
C: sem_wait(full)→0,  item=0,      sem_post(empty)→2
P: sem_wait(empty)→1, buffer[1]=1, sem_post(full)→1
P: sem_wait(empty)→0, buffer[0]=2, sem_post(full)→2
C: sem_wait(full)→1,  item=1,      sem_post(empty)→1
C: sem_wait(full)→0,  item=2,      sem_post(empty)→2
```

---

## 6. Implémentation dans mthread

```c
typedef struct {
    int value;
    mthread_struct_t *wait_queue;
} mthread_sem_t;
```

**sem_wait** :
```c
int mthread_sem_wait(mthread_sem_t *s) {
    if (s->value > 0) { s->value--; return 0; }
    // bloquer
    mthread_struct_t *me = _mthread_current();
    /* ajouter me à s->wait_queue */
    me->state = MTHREAD_BLOCKED;
    _mthread_schedule();
    return 0;  // réveillé quand un jeton disponible
}
```

**sem_post** :
```c
int mthread_sem_post(mthread_sem_t *s) {
    if (!s->wait_queue) { s->value++; return 0; }
    // réveiller un thread en attente
    mthread_struct_t *next = s->wait_queue;
    s->wait_queue = next->next;
    _mthread_enqueue(next);
    // NE PAS incrémenter s->value : le thread réveillé "prend" le jeton
    return 0;
}
```

---

## Résumé

| Concept         | À retenir                                           |
|-----------------|-----------------------------------------------------|
| sem_wait        | Décrémenter si >0, sinon bloquer                    |
| sem_post        | Incrémenter si personne attend, sinon réveiller     |
| Pas d'ownership | N'importe quel thread peut faire post               |
| Prod/conso      | 2 sémaphores : sem_empty (init=N) + sem_full (init=0) |
| Implémentation  | Compteur + file d'attente, comme le mutex           |
