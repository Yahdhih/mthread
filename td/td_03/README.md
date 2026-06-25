# TD 03 — Cycle de vie d'un thread

> Exercices sur papier. Durée estimée : 1h30.

---

## Exercice 1 — Diagramme d'états

a) Dessine la machine à états complète d'un thread avec les 4 états : READY, RUNNING, BLOCKED, ZOMBIE. Pour chaque transition, indique l'événement qui la déclenche.

b) Est-il possible d'aller directement de READY à ZOMBIE ? De BLOCKED à RUNNING ?

c) Un thread en état ZOMBIE consomme-t-il du CPU ? De la mémoire ?

---

## Exercice 2 — Trace d'exécution

Considère ce programme (pseudo-code) :

```
T_main crée T1 (exécute worker1)
T_main crée T2 (exécute worker2)
T_main appelle join(T1)

worker1 : attend un mutex M (M est libre)
          travaille 5s
          libère M
          retourne 42

worker2 : attend le mutex M (M est pris par T1)
          travaille 2s
          libère M
          retourne 7
```

Trace l'état de chaque thread (T_main, T1, T2) à chaque moment clé :
1. Après la création de T1 et T2
2. Quand T1 acquiert M
3. Quand T2 tente d'acquérir M
4. Quand T1 libère M
5. Quand T1 retourne
6. Quand T_main revient de `join(T1)`

---

## Exercice 3 — Problèmes de cycle de vie

Identifie le bug dans chaque extrait et propose la correction :

**Fragment A :**
```c
void *worker(void *arg) {
    return malloc(100);   // retourne un pointeur alloué sur le heap
}

int main() {
    mthread_t t;
    mthread_create(&t, worker, NULL);
    // ... (pas de join)
    return 0;
}
```

**Fragment B :**
```c
void *worker(void *arg) {
    int result = 42;
    return &result;       // retourne un pointeur vers une variable locale
}

int main() {
    mthread_t t;
    void *retval;
    mthread_create(&t, worker, NULL);
    mthread_join(t, &retval);
    printf("%d\n", *(int *)retval);  // utilise la valeur de retour
}
```

**Fragment C :**
```c
mthread_t t;
mthread_create(&t, worker, NULL);
mthread_join(t, NULL);
mthread_join(t, NULL);   // double join
```

---

## Exercice 4 — `uc_link` et terminaison

Dans `mthread_create`, on configure `ctx.uc_link = &scheduler_ctx`.

a) Que se passe-t-il quand la fonction du thread retourne, grâce à `uc_link` ?

b) Si on mettait `uc_link = NULL`, que se passerait-il quand le thread retourne ?

c) Pourquoi ne peut-on pas utiliser directement `uc_link = &ctx_main` (le contexte du thread qui a appelé `create`) ?

d) Propose une implémentation de `mthread_exit(retval)` sachant qu'elle doit :
   - Sauvegarder la valeur de retour
   - Passer le thread en état ZOMBIE
   - Réveiller le thread qui fait `join` (s'il y en a un)
   - Rendre la main au scheduler

---

## Exercice 5 — Jointure et synchronisation

```c
int résultat_global = 0;

void *calculer(void *arg) {
    résultat_global = 42;
    return NULL;
}

int main() {
    mthread_t t;
    mthread_create(&t, calculer, NULL);
    printf("%d\n", résultat_global);   // (A) — sans join
    mthread_join(t, NULL);
    printf("%d\n", résultat_global);   // (B) — après join
}
```

a) Que peut afficher la ligne (A) ? Pourquoi ?

b) Que garantit la ligne (B) ? Pourquoi ?

c) Le `join` joue-t-il un rôle de synchronisation ? Quelles garanties apporte-t-il ?

---

## Corrigé

<details>
<summary>Exercice 3</summary>

**Fragment A** : Bug — pas de `join`. Le thread principal peut se terminer avant T1, libérant le processus. La mémoire allouée par `malloc` dans le thread sera perdue (leak). Correction : ajouter `mthread_join(t, NULL)` et libérer la valeur de retour.

**Fragment B** : Bug grave — retourner un pointeur vers une variable locale. La variable `result` est sur la pile du thread. Quand le thread se termine, la pile est potentiellement réutilisée. Le pointeur devient dangereux. Correction : utiliser `malloc` pour la valeur de retour.

**Fragment C** : Bug — double join sur le même thread. Après le premier `join`, le thread est libéré (état ZOMBIE détruit). Le second `join` accède à de la mémoire libérée → comportement indéfini. Correction : joindre une seule fois.

</details>

<details>
<summary>Exercice 5</summary>

a) La ligne (A) peut afficher `0` (le thread n'a pas encore eu le CPU) ou `42` (le thread a eu le CPU et a écrit). L'ordre n'est pas garanti → race condition.

b) Après `join`, le thread T est garanti d'être terminé. Donc `résultat_global == 42` est garanti. (A) peut afficher 0 ou 42 ; (B) affiche toujours 42.

c) Oui, `join` est une barrière de synchronisation. Il garantit que toutes les modifications faites par le thread avant son retour sont visibles par le thread qui joint.

</details>
