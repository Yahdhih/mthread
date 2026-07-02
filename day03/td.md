# TD01 — Exercice 1 : La pile d'appel

**Durée estimée : 15 min**

---

## Exercice 1 — Tracer la pile d'appel

Considère le programme C suivant :

```c
#include <stdio.h>

void c(int z) {
    printf("dans c, z = %d\n", z);
    // point d'observation C
}

void b(int y) {
    printf("dans b, y = %d\n", y);
    c(y + 10);
    // point d'observation B-retour
}

void a(int x) {
    printf("dans a, x = %d\n", x);
    b(x * 2);
    // point d'observation A-retour
}

int main(void) {
    a(3);
    // point d'observation MAIN
    return 0;
}
```

### Question 1

Trace l'état de la pile au **point d'observation C** (juste avant que `printf` s'exécute dans `c`).

Pour chaque cadre de pile (*stack frame*), indique :
- le nom de la fonction
- les variables locales et paramètres avec leurs valeurs
- l'adresse de retour (tu peux noter "→ appelant")

```
Sommet de pile (adresses basses)
┌──────────────────────────────┐
│  ???                         │  ← frame de ???
│  ...                         │
├──────────────────────────────┤
│  ???                         │  ← frame de ???
│  ...                         │
├──────────────────────────────┤
│  ???                         │
│  ...                         │
├──────────────────────────────┤
│  ???                         │
│  ...                         │
└──────────────────────────────┘
Bas de pile (adresses hautes)
```

**Ta réponse ici :**

```
Sommet de pile (adresses basses)
┌──────────────────────────────┐
│  z = 16                      │  ← frame de c()
│  adresse retour → b+offset   │
├──────────────────────────────┤
│  y = 6                       │  ← frame de b()
│  adresse retour → a+offset   │
├──────────────────────────────┤
│  x = 3                       │  ← frame de a()
│  adresse retour → main+offset│
├──────────────────────────────┤
│  (variables locales de main) │  ← frame de main()
│  adresse retour → _start     │
└──────────────────────────────┘
Bas de pile (adresses hautes)

```

---

### Question 2

Qu'affiche exactement le programme ? Écris la sortie ligne par ligne.

```
[espace de réponse]
```

---

### Question 3

Lorsque `c` se termine et que `b` reprend l'exécution au point d'observation B-retour, que contient la pile ? Le cadre de `c` est-il encore là ?

```
[espace de réponse]
```

---

### Question 4 (bonus)

Si `c` appelait `malloc(100)`, où est allouée la mémoire ? Est-ce sur la pile ? Que se passe-t-il si on ne fait pas `free` avant de retourner de `c` ?

```
[espace de réponse]
```

---

<details>
<summary>Corrigé — clique pour révéler</summary>

### Corrigé Q1 — État de la pile au point C

La pile croît vers les adresses basses. Chaque appel de fonction empile un nouveau cadre.

```
Sommet de pile (adresses basses)
┌──────────────────────────────┐
│  z = 16                      │  ← frame de c()
│  adresse retour → b+offset   │
├──────────────────────────────┤
│  y = 6                       │  ← frame de b()
│  adresse retour → a+offset   │
├──────────────────────────────┤
│  x = 3                       │  ← frame de a()
│  adresse retour → main+offset│
├──────────────────────────────┤
│  (variables locales de main) │  ← frame de main()
│  adresse retour → _start     │
└──────────────────────────────┘
Bas de pile (adresses hautes)
```

**Calcul des valeurs :**
- `main` appelle `a(3)` → `x = 3`
- `a` appelle `b(x * 2)` = `b(6)` → `y = 6`
- `b` appelle `c(y + 10)` = `c(16)` → `z = 16`

### Corrigé Q2 — Sortie du programme

```
dans a, x = 3
dans b, y = 6
dans c, z = 16
```

### Corrigé Q3 — Pile après retour de c

Quand `c` retourne, son cadre est **dépilé** (le pointeur de pile `rsp`/`sp` est avancé vers les adresses hautes). La mémoire n'est pas effacée — elle existe encore physiquement — mais elle est considérée comme libre et sera écrasée au prochain appel.

Au point B-retour, la pile contient :
```
Sommet de pile
┌──────────────────────────────┐
│  y = 6                       │  ← frame de b() (de nouveau au sommet)
│  adresse retour → a+offset   │
├──────────────────────────────┤
│  x = 3                       │  ← frame de a()
│  ...                         │
└──────────────────────────────┘
```

Le cadre de `c` est **gone** du point de vue du programme. Accéder à une variable locale de `c` depuis `b` serait un comportement indéfini.

### Corrigé Q4 — malloc

`malloc(100)` alloue sur le **tas** (*heap*), pas sur la pile. Le tas est une zone mémoire séparée gérée par l'allocateur. Le pointeur retourné par `malloc` est stocké dans une variable locale (sur la pile), mais les 100 octets sont sur le tas.

Si on ne fait pas `free` avant de retourner de `c` : la variable locale qui contenait le pointeur est perdue (le cadre de `c` est dépilé), mais les 100 octets du tas sont toujours alloués. C'est une **fuite mémoire** (*memory leak*). Le programme continue de fonctionner, mais cette mémoire ne sera jamais libérée jusqu'à la fin du processus.

</details>

---

➡️ Demain : `day04/` — TD02 : swapcontext en détail
