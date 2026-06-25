# TD 04 — Mutex et Exclusion Mutuelle

> Exercices sur papier. Durée estimée : 2h.

---

## Exercice 1 — Identifier les races conditions

Pour chaque extrait, dis s'il y a une race condition, pourquoi, et comment la corriger.

**Fragment A :**
```c
int solde = 1000;

void *retrait(void *arg) {
    int montant = *(int *)arg;
    if (solde >= montant) {
        solde -= montant;
        printf("Retrait OK : %d\n", montant);
    } else {
        printf("Solde insuffisant\n");
    }
    return NULL;
}
// T1 retire 800, T2 retire 800, solde initial = 1000
```

**Fragment B :**
```c
int flag = 0;

void *setter(void *arg) {
    flag = 1;
    return NULL;
}

void *waiter(void *arg) {
    while (flag == 0);   // attente active
    printf("Flag mis à 1\n");
    return NULL;
}
```

**Fragment C :**
```c
typedef struct { int x; int y; } Point;
Point p = {0, 0};

void *move(void *arg) {
    p.x = 5;
    p.y = 5;
    return NULL;
}

void *print_point(void *arg) {
    printf("(%d, %d)\n", p.x, p.y);
    return NULL;
}
```

---

## Exercice 2 — Sections critiques

Identifie la section critique minimale dans chaque cas (le moins de code possible à protéger) :

**Cas 1 :** Un compteur de visites sur un serveur web. Chaque thread traite une requête et incrémente le compteur.

```c
void *traiter_requete(void *arg) {
    préparer_réponse();       // 50ms — indépendant
    compteur_visites++;       // section critique
    envoyer_réponse();        // 30ms — indépendant
    return NULL;
}
```

**Cas 2 :** Ajout d'un élément dans une liste chaînée partagée.

```c
void liste_ajouter(Liste *l, int val) {
    Noeud *n = malloc(sizeof(Noeud));  // ← protéger ?
    n->val = val;                       // ← protéger ?
    n->next = l->tete;                 // ← protéger ?
    l->tete = n;                       // ← protéger ?
}
```

---

## Exercice 3 — Deadlock

Analyse chaque scénario et détermine s'il y a deadlock :

**Scénario A :**
```
T1 : lock(A) → lock(B) → unlock(B) → unlock(A)
T2 : lock(A) → lock(B) → unlock(B) → unlock(A)
```

**Scénario B :**
```
T1 : lock(A) → lock(B) → unlock(B) → unlock(A)
T2 : lock(B) → lock(A) → unlock(A) → unlock(B)
```

**Scénario C :**
```
T1 : lock(A) → lock(A)   (mutex non-récursif)
```

**Scénario D :**
```
T1 : lock(A) → lock(B) → unlock(B) → lock(C) → unlock(C) → unlock(A)
T2 : lock(B) → lock(C) → unlock(C) → unlock(B)
T3 : lock(C) → lock(A) → unlock(A) → unlock(C)
```

Pour les scénarios avec deadlock, dessine le graphe d'attente (Resource Allocation Graph) et identifie le cycle.

---

## Exercice 4 — Prévention du deadlock

**Règle de prévention** : acquérir les verrous dans un ordre global total.

On a 3 mutex : M_compte, M_historique, M_stats. On leur assigne des numéros :
- M_compte = 1
- M_historique = 2
- M_stats = 3

Ré-écris ces fonctions pour respecter l'ordre global :

```c
void virement(Compte *src, Compte *dst, int montant) {
    mutex_lock(&dst->mutex);    // ← ordre incorrect ?
    mutex_lock(&src->mutex);
    src->solde -= montant;
    dst->solde += montant;
    mutex_unlock(&src->mutex);
    mutex_unlock(&dst->mutex);
}
```

**Problème** : deux threads font `virement(A, B)` et `virement(B, A)` simultanément. Y a-t-il deadlock ? Comment corriger ?

---

## Exercice 5 — Analyse de performance

Un programme a une section critique de durée `t_cs` et une section non-critique de durée `t_nc`. N threads s'exécutent en boucle.

a) Quelle est la fraction maximale du temps où le programme peut être parallèle ?

b) Si `t_cs = 1ms` et `t_nc = 99ms`, quel est le speedup maximum avec 4 threads (loi d'Amdahl) ?

c) Si `t_cs = 50ms` et `t_nc = 50ms`, quel est le speedup maximum avec 4 threads ?

d) Quelle conclusion tires-tu sur la taille des sections critiques et les performances ?

**Rappel — Loi d'Amdahl** : `Speedup = 1 / (s + (1-s)/N)` où s est la fraction séquentielle et N le nombre de threads.

---

## Corrigé

<details>
<summary>Exercice 1 — Fragment A</summary>

Race condition sur `solde`. Deux threads lisent `solde >= montant` (true pour les deux car solde=1000), puis tous les deux retirent 800. Résultat : solde = -600 alors qu'il devrait rester ≥ 0.

```c
mthread_mutex_t m = MTHREAD_MUTEX_INIT;
void *retrait(void *arg) {
    int montant = *(int *)arg;
    mutex_lock(&m);
    if (solde >= montant) {
        solde -= montant;
        printf("OK\n");
    }
    mutex_unlock(&m);
    return NULL;
}
```

</details>

<details>
<summary>Exercice 3</summary>

**A** : Pas de deadlock — même ordre d'acquisition (A→B).

**B** : Deadlock possible. T1 a A, T2 a B. T1 attend B, T2 attend A → cycle.

**C** : Deadlock immédiat — T1 essaie de verrouiller A qu'il détient déjà (mutex non-récursif).

**D** : Deadlock possible. Cycle : T1 tient A attend B, T2 tient B attend C, T3 tient C attend A.

</details>

<details>
<summary>Exercice 5</summary>

b) s = t_cs/(t_cs + t_nc) = 1/100 = 0.01. Speedup max(4) = 1/(0.01 + 0.99/4) ≈ 3.88.

c) s = 50/100 = 0.5. Speedup max(4) = 1/(0.5 + 0.5/4) = 1/0.625 = 1.6.

d) Plus la section critique est longue, plus le parallélisme est limité. Minimiser la section critique est crucial.

</details>
