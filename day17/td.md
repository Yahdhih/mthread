# TD — Exercice 1 : Trace Producteur/Consommateur

---

## Exercice 1 — Trace d'exécution

**Configuration** :
- Buffer circulaire de taille N=3
- 1 producteur : produit les items 0, 1, 2, 3, 4
- 2 consommateurs C1 et C2 : chacun consomme 2 items (C1 et C2 se partagent les 4 premiers)
- `sem_empty` initialisé à 3, `sem_full` initialisé à 0

**a)** Remplis la trace ci-dessous en notant les valeurs de `sem_empty` et `sem_full` après chaque opération :

```
                              | sem_empty | sem_full |
Départ                        |     3     |    0     |
P: sem_wait(empty)            |           |          |
P: buffer[0]=0                |           |          |
P: sem_post(full)             |           |          |
C1: sem_wait(full)            |           |          |
C1: item=buffer[0]=0          |           |          |
C1: sem_post(empty)           |           |          |
P: sem_wait(empty)            |           |          |
P: buffer[1]=1                |           |          |
P: sem_post(full)             |           |          |
P: sem_wait(empty)            |           |          |
P: buffer[2]=2                |           |          |
P: sem_post(full)             |           |          |
P: sem_wait(empty)            |           |          |
P: buffer[0]=3  [buffer plein]|           |          |
P: sem_post(full)             |           |          |
```

**b)** Si le producteur essaie de produire un 4ème item alors que le buffer est plein (et que les consommateurs sont lents), que se passe-t-il ? Quel sémaphore le bloque ?

**c)** Peut-on avoir `sem_empty + sem_full > N` ? Justifier.

---

**Espace de réponse :**
```
a) (compléter la table)

b)

c)
```

---

## Corrigé

<details>
<summary>Exercice 1</summary>

a)
```
                              | sem_empty | sem_full |
Départ                        |     3     |    0     |
P: sem_wait(empty)            |     2     |    0     |
P: buffer[0]=0                |     2     |    0     |
P: sem_post(full)             |     2     |    1     |
C1: sem_wait(full)            |     2     |    0     |
C1: item=buffer[0]=0          |     2     |    0     |
C1: sem_post(empty)           |     3     |    0     |
P: sem_wait(empty)            |     2     |    0     |
P: buffer[1]=1                |     2     |    0     |
P: sem_post(full)             |     2     |    1     |
P: sem_wait(empty)            |     1     |    1     |
P: buffer[2]=2                |     1     |    1     |
P: sem_post(full)             |     1     |    2     |
P: sem_wait(empty)            |     0     |    2     |
P: buffer[0]=3                |     0     |    2     |
P: sem_post(full)             |     0     |    3     |
```

b) Le producteur appelle `sem_wait(empty)` alors que `sem_empty == 0` → il se bloque. Il ne sera réveillé que quand un consommateur appellera `sem_post(empty)`.

c) Non. À tout moment, `sem_empty + sem_full ≤ N`. En effet, `sem_empty` compte les places vides et `sem_full` compte les places occupées. Leur somme est le nombre de cases, qui vaut N. (Égal à N quand personne n'est en train d'écrire/lire.)

</details>
