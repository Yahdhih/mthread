# TD 01 — Processus et Threads : Fondements

> Exercices sur papier. Durée estimée : 1h30.

---

## Exercice 1 — Vrai ou Faux (justifier)

Pour chaque affirmation, dire si elle est vraie ou fausse et expliquer pourquoi.

1. Deux processus distincts peuvent lire la même adresse mémoire virtuelle `0x7fff1234` et obtenir la même valeur.

2. Si T1 et T2 sont deux threads du même processus, une variable locale de T1 est visible par T2.

3. Un crash dans le thread T1 d'un processus P peut tuer tous les threads de P.

4. La création d'un thread est moins coûteuse que la création d'un processus.

5. Dans le modèle N:1, si un thread fait un appel système bloquant (ex: lire un fichier), tous les threads du processus sont bloqués.

---

## Exercice 2 — Schéma mémoire

Considère le programme suivant :

```c
#include <stdio.h>
#include <stdlib.h>

int globale = 42;           // (A)

void *worker(void *arg) {
    int locale = 10;        // (B)
    int *heap = malloc(4);  // (C)
    *heap = 99;
    return NULL;
}

int main() {
    int main_locale = 5;    // (D)
    // créer 2 threads qui exécutent worker()
    ...
}
```

**Questions :**

a) Remplis le tableau suivant pour les variables A, B, C, D :

| Variable | Segment mémoire | Partagée entre threads ? |
|----------|-----------------|--------------------------|
| A        |                 |                          |
| B        |                 |                          |
| C (pointeur) |             |                          |
| *heap (valeur) |           |                          |
| D        |                 |                          |

b) Les deux threads exécutent `worker()` simultanément. Peuvent-ils avoir deux valeurs différentes pour `locale` ? Pourquoi ?

c) Si T1 fait `globale = 100` et T2 lit ensuite `globale`, quelle valeur lit-il ?

---

## Exercice 3 — Parallélisme vs Concurrence

**Situation A** : Machine avec 1 cœur CPU, 4 threads.
**Situation B** : Machine avec 4 cœurs CPU, 4 threads.
**Situation C** : Machine avec 2 cœurs CPU, 4 threads.

Pour chaque situation :
1. Y a-t-il parallélisme ? Concurrence ?
2. Combien de threads peuvent s'exécuter **simultanément** au sens strict ?
3. Dessine un diagramme temporel montrant l'exécution sur 8 unités de temps.

---

## Exercice 4 — Modèles de threads

a) Dans le modèle **1:1** (kernel threads), si T1 fait un appel système bloquant (`read()`), que se passe-t-il pour T2 ?

b) Dans le modèle **N:1** (user threads), même question.

c) En C avec `pthread`, quel modèle est utilisé sur Linux ?

d) Notre bibliothèque `mthread` utilisera le modèle N:1. Donne **deux avantages** et **deux inconvénients** de ce choix.

---

## Exercice 5 — Question de réflexion

Un serveur web reçoit 1000 requêtes simultanées. Pour chaque requête, il lit un fichier sur disque (opération lente) puis envoie une réponse.

a) Pourquoi utiliser des threads plutôt qu'un seul processus séquentiel ?

b) Pourquoi un serveur réel utiliserait-il le modèle 1:1 (pthreads) plutôt que N:1 (mthread) dans ce cas ?

c) Quelle alternative aux threads existe pour gérer l'I/O sans créer un thread par connexion ? (Indice : `epoll`, `select`)

---

## Corrigé — à ne regarder qu'après avoir tenté les exercices

<details>
<summary>Exercice 1</summary>

1. **Faux** — Les adresses virtuelles sont identiques mais mappées à des pages physiques différentes. Valeurs indépendantes.
2. **Faux** — Une variable **locale** est sur la pile du thread. Chaque thread a sa propre pile. T2 ne peut pas y accéder (sauf si T1 lui passe un pointeur, mais c'est un usage explicite et dangereux).
3. **Vrai** — Un crash (ex: segfault) envoie un signal au processus entier. Par défaut, le processus est tué avec tous ses threads.
4. **Vrai** — Créer un thread alloue une pile et initialise un contexte. Créer un processus (`fork`) duplique tout l'espace d'adressage (ou le marque copy-on-write), met à jour les tables du noyau, etc.
5. **Vrai** — Dans N:1, le noyau ne voit qu'un seul thread. Si ce thread est bloqué par le noyau, tous les threads utilisateur sont gelés.

</details>

<details>
<summary>Exercice 2</summary>

| Variable   | Segment    | Partagée ?             |
|------------|------------|------------------------|
| A          | .data      | Oui — tous les threads |
| B          | Stack T1/T2 | Non — pile privée de chaque thread |
| C (pointeur) | Stack    | Non — pointeur local, non partagé |
| *heap      | Heap       | Oui — si le pointeur est partagé |
| D          | Stack main | Non — pile du main thread |

b) Oui, les deux threads ont leur propre pile, donc leur propre copie de `locale`. Deux valeurs distinctes coexistent.

c) T2 lit `100`. La variable globale est partagée. Attention : sans synchronisation, l'ordre des accès n'est pas garanti.

</details>
