# TD 06 — Variables de Condition

> Exercices sur papier. Durée estimée : 2h.

---

## Exercice 1 — `if` vs `while`

Considère ce code avec `if` :

```c
int disponible = 0;
mthread_mutex_t m;
mthread_cond_t  cond;

void *consommateur(void *arg) {
    mutex_lock(&m);
    if (disponible == 0)         // ← if, pas while
        cond_wait(&cond, &m);
    consommer();                 // utilise la ressource
    disponible = 0;
    mutex_unlock(&m);
    return NULL;
}

void *producteur(void *arg) {
    mutex_lock(&m);
    disponible = 1;
    cond_signal(&cond);
    mutex_unlock(&m);
    return NULL;
}
```

**Scénario problématique** : 2 consommateurs C1, C2 et 1 producteur P. La ressource ne peut être consommée qu'une seule fois.

a) Trace l'exécution suivante et montre le bug :
   1. C1 et C2 appellent `cond_wait` (les deux attendent)
   2. P produit et appelle `cond_broadcast`
   3. C1 et C2 sont réveillés
   
b) Que se passe-t-il si on remplace `if` par `while` ?

c) Quel est le spurious wakeup ? Montre un scénario où `while` protège mais pas `if`.

---

## Exercice 2 — Problème lecteurs/écrivains

**Énoncé** : une base de données peut être lue par plusieurs threads simultanément, mais écrite par un seul à la fois (exclusion avec les lecteurs et les autres écrivains).

**Variables :**
```c
int nb_lecteurs = 0;
mthread_mutex_t m;
mthread_cond_t  peut_lire;
mthread_cond_t  peut_ecrire;
int en_ecriture = 0;
```

a) Complète le pseudo-code de `debut_lecture()`, `fin_lecture()`, `debut_ecriture()`, `fin_ecriture()`.

b) Y a-t-il un risque de starvation pour les écrivains ? Dans quel scénario ?

c) Propose une modification pour donner priorité aux écrivains en attente.

---

## Exercice 3 — `cond_wait` atomique

Pourquoi est-il **absolument nécessaire** que `cond_wait` libère le mutex et bloque de manière **atomique** ?

Montre un scénario (avec un interleaving précis) où la non-atomicité perd un signal si :
- T1 appelle `mutex_unlock(&m)` séparément
- Puis T1 appelle `cond_wait_no_unlock(&cond)` (version non-atomique)

```
T1 (consommateur)         T2 (producteur)
mutex_unlock(&m)   ←─── interleaving possible ici
                          mutex_lock(&m)
                          produire()
                          cond_signal(&cond)   ← signal perdu !
                          mutex_unlock(&m)
cond_wait_no_unlock(...)  ← attend pour toujours
```

---

## Exercice 4 — Barrière avec variables de condition

Implémente une **barrière** pour N threads : tous les threads doivent appeler `barrier_wait()` avant que l'un d'eux puisse continuer.

```c
typedef struct {
    int             total;    // nombre de threads attendus
    int             arrivés;  // nombre arrivés jusqu'ici
    mthread_mutex_t mutex;
    mthread_cond_t  cond;
} Barrière;
```

a) Implémente `barrière_init(Barrière *b, int n)`.

b) Implémente `barrière_wait(Barrière *b)`.

c) Y a-t-il un problème si la barrière est **réutilisable** (les threads la traversent plusieurs fois) ? Comment le résoudre ?

---

## Exercice 5 — Pipeline de threads

3 threads forment un pipeline : T1 produit des données, T2 les transforme, T3 les affiche.

```
T1 → [buffer_1] → T2 → [buffer_2] → T3
```

Chaque buffer contient au plus 1 élément (pour simplifier).

a) Identifie les variables de condition nécessaires.

b) Écris le pseudo-code de chaque thread.

c) Que se passe-t-il si T3 est beaucoup plus lent que T1 ? Comment modifier le système pour limiter la vitesse de T1 ?

---

## Corrigé

<details>
<summary>Exercice 2 — Lecteurs/Écrivains</summary>

```c
void debut_lecture() {
    mutex_lock(&m);
    while (en_ecriture)
        cond_wait(&peut_lire, &m);
    nb_lecteurs++;
    mutex_unlock(&m);
}

void fin_lecture() {
    mutex_lock(&m);
    nb_lecteurs--;
    if (nb_lecteurs == 0)
        cond_signal(&peut_ecrire);
    mutex_unlock(&m);
}

void debut_ecriture() {
    mutex_lock(&m);
    while (en_ecriture || nb_lecteurs > 0)
        cond_wait(&peut_ecrire, &m);
    en_ecriture = 1;
    mutex_unlock(&m);
}

void fin_ecriture() {
    mutex_lock(&m);
    en_ecriture = 0;
    cond_broadcast(&peut_lire);   // réveiller tous les lecteurs
    cond_signal(&peut_ecrire);    // ou un écrivain
    mutex_unlock(&m);
}
```

Starvation des écrivains : si des lecteurs arrivent continuellement, un écrivain peut attendre indéfiniment.

</details>

<details>
<summary>Exercice 4</summary>

```c
void barrière_wait(Barrière *b) {
    mutex_lock(&b->mutex);
    b->arrivés++;
    if (b->arrivés == b->total) {
        b->arrivés = 0;                  // réinitialiser pour réutilisation
        cond_broadcast(&b->cond);        // réveiller tout le monde
    } else {
        while (b->arrivés != 0 && b->arrivés < b->total)
            cond_wait(&b->cond, &b->mutex);
    }
    mutex_unlock(&b->mutex);
}
```

Problème réutilisation : le thread le plus rapide peut entrer dans la barrière suivante avant que les autres soient sortis. Solution : utiliser un compteur de génération (parity/phase).

</details>
