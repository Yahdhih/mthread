# Cours — Variables de Condition (sections 1-3)

---

## 1. Limitation du Mutex Seul

**Problème** : un consommateur veut attendre qu'il y ait des éléments dans le buffer.

Avec mutex seul, on est forcé de faire de **l'attente active** :

```c
// Mauvaise solution — busy wait
void consommer(void) {
    while (1) {
        mthread_mutex_lock(&mu);
        if (count > 0) break;         // quelque chose à consommer ?
        mthread_mutex_unlock(&mu);
        mthread_yield();              // relâcher et réessayer plus tard
    }
    // consommer...
    mthread_mutex_unlock(&mu);
}
```

**Problèmes** :
- Gaspillage CPU : le thread tourne en boucle sans rien faire d'utile
- Si le producteur est lent, le consommateur yield des centaines de fois pour rien
- Code compliqué et fragile

**Ce qu'on voudrait** : "bloquer jusqu'à ce que la condition `count > 0` soit vraie".

---

## 2. Variables de Condition

### Définition 2.1 — Variable de Condition
Une **variable de condition** permet à un thread de se bloquer en attendant qu'une **condition** (portant sur des données partagées) devienne vraie, et d'être réveillé quand un autre thread la satisfait.

Trois opérations :

### `cond_wait(c, m)` — Attendre la condition
```c
int mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m);
```
- Ajoute le thread à la file d'attente de `c`
- **Relâche** le mutex `m` (atomiquement avec le blocage)
- Bloque le thread courant
- Quand réveillé : **réacquiert** le mutex `m` avant de retourner

### `cond_signal(c)` — Réveiller un thread
```c
int mthread_cond_signal(mthread_cond_t *c);
```
- Réveille **un** thread en attente sur `c` (le premier dans la file)
- Si aucun thread n'attend : ne fait rien

### `cond_broadcast(c)` — Réveiller tous les threads
```c
int mthread_cond_broadcast(mthread_cond_t *c);
```
- Réveille **tous** les threads en attente sur `c`

---

## 3. L'Invariant d'Atomicité de `cond_wait`

### Théorème 3.1 — Atomicité de cond_wait
**L'unlock du mutex et la mise en attente doivent être atomiques.**

**Pourquoi ?** Imagine ce bug :

```
// MAUVAISE implémentation de cond_wait (non atomique)
void cond_wait_BUGGE(cond_t *c, mutex_t *m) {
    mutex_unlock(m);         // (1) relâcher le mutex
                             // ← SIGNAL PERDU possible ici !
    block_on(c);             // (2) se bloquer
}

// Scénario de signal perdu :
Consommateur : cond_wait → étape (1) : unlock(mu)
Producteur   : produit un item
Producteur   : cond_signal(c)  → personne n'attend encore !
Consommateur : étape (2) : block_on(c)  → se bloque pour toujours
```

**Solution** : les deux étapes doivent être atomiques (inséparables).

Dans notre bibliothèque :
```c
int mthread_cond_wait(mthread_cond_t *c, mthread_mutex_t *m) {
    // Ordre CORRECT :
    // 1. Ajouter à la file d'attente (avant de relâcher le mutex)
    mthread_struct_t *me = _mthread_current();
    /* ... ajouter me à c->wait_queue ... */
    me->state = MTHREAD_BLOCKED;

    // 2. Relâcher le mutex
    mthread_mutex_unlock(m);

    // 3. Passer la main (dormir)
    _mthread_schedule();

    // 4. Réacquérir le mutex avant de retourner
    mthread_mutex_lock(m);
    return 0;
}
```

**Pourquoi ça fonctionne ?** Le signal cherche dans `c->wait_queue`. Si le thread y est déjà (étape 1), le signal le trouvera même si le thread n'a pas encore appelé `_mthread_schedule`. Dans notre scheduler coopératif N:1 mono-thread, ce problème est simplifié (pas de vrai parallélisme), mais l'ordre reste critique pour la correction logique.

---

## Résumé des 3 premières sections

| Concept          | À retenir                                               |
|------------------|---------------------------------------------------------|
| Busy wait        | Mauvais : gaspille CPU, code fragile                    |
| cond_wait        | Bloque + unlock atomique, réacquiert au réveil          |
| cond_signal      | Réveille un waiter, ne fait rien si aucun               |
| cond_broadcast   | Réveille tous les waiters                               |
| Atomicité        | Ajouter à la queue AVANT de relâcher le mutex           |
