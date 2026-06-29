# TD02 — Exercice 5 : Jeton entre 3 contextes

---

## Exercice 5 — Le jeton circulaire

Voici un programme avec 3 contextes qui se passent un "jeton" via `swapcontext` en boucle :

```c
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE (64 * 1024)
#define N_TOURS 2

static ucontext_t ctx_main, ctx_a, ctx_b, ctx_c;
static int jeton = 0;

void thread_c(void) {
    for (int i = 0; i < N_TOURS; i++) {
        jeton++;
        printf("[C] jeton = %d\n", jeton);
        swapcontext(&ctx_c, &ctx_a);  // passe à A
    }
    swapcontext(&ctx_c, &ctx_main);   // retourne au main
}

void thread_b(void) {
    for (int i = 0; i < N_TOURS; i++) {
        jeton++;
        printf("[B] jeton = %d\n", jeton);
        swapcontext(&ctx_b, &ctx_c);  // passe à C
    }
    swapcontext(&ctx_b, &ctx_main);   // retourne au main
}

void thread_a(void) {
    for (int i = 0; i < N_TOURS; i++) {
        jeton++;
        printf("[A] jeton = %d\n", jeton);
        swapcontext(&ctx_a, &ctx_b);  // passe à B
    }
    swapcontext(&ctx_a, &ctx_main);   // retourne au main
}

int main(void) {
    // [initialisation des contextes omise pour l'exercice]
    printf("[Main] départ\n");
    swapcontext(&ctx_main, &ctx_a);   // lance A
    printf("[Main] fin, jeton = %d\n", jeton);
    return 0;
}
```

### Question 5.1

Quelle est la sortie complète du programme (pour `N_TOURS = 2`) ? Trace l'exécution.

```
[espace de réponse]
```

### Question 5.2

Combien de `swapcontext` sont nécessaires pour qu'un "tour complet" (A → B → C → A) se produise ?

```
[espace de réponse]
```

### Question 5.3

Que se passe-t-il si on oublie de mettre `uc_link` lors de l'initialisation des contextes, **et** que les fonctions retournent sans `swapcontext` final (sans les lignes `swapcontext(&ctx_X, &ctx_main)` en fin de boucle) ?

```
[espace de réponse]
```

### Question 5.4 (bonus)

Si `N_TOURS = 2`, la valeur finale de `jeton` est-elle prévisible ? Justifie. Serait-ce encore le cas avec de vrais threads (noyau) ?

```
[espace de réponse]
```

<details>
<summary>Corrigé exercice 5</summary>

**Q5.1 — Sortie pour N_TOURS = 2 :**

```
[Main] départ
[A] jeton = 1
[B] jeton = 2
[C] jeton = 3
[A] jeton = 4
[B] jeton = 5
[C] jeton = 6
[Main] fin, jeton = 6
```

**Trace d'exécution :**
```
main  → swapcontext(&ctx_main, &ctx_a)     ← lance A
A(i=0)→ jeton++ (=1), affiche, swap → B
B(i=0)→ jeton++ (=2), affiche, swap → C
C(i=0)→ jeton++ (=3), affiche, swap → A
A(i=1)→ jeton++ (=4), affiche, swap → B
B(i=1)→ jeton++ (=5), affiche, swap → C
C(i=1)→ jeton++ (=6), affiche, swap → main (fin de boucle)
main  → affiche "fin, jeton = 6"
```

Note : quand C fait `swapcontext(&ctx_c, &ctx_main)`, main reprend juste après son `swapcontext`. Mais A et B ont aussi fini leur boucle sans repasser par main. Dans ce code tel qu'écrit, B et C font `swapcontext(&ctx_b/c, &ctx_main)` en fin de boucle — mais après le premier tour, A repasse en boucle. L'ordre exact dépend de combien de tours chacun fait. Ici tous font 2 tours, donc la chaîne est bien A→B→C→A→B→C, puis C retourne à main.

**Q5.2 — Swapcontext par tour :**

Un tour complet (A → B → C → retour à A) nécessite **3 swapcontext** :
1. A → B : `swapcontext(&ctx_a, &ctx_b)`
2. B → C : `swapcontext(&ctx_b, &ctx_c)`
3. C → A : `swapcontext(&ctx_c, &ctx_a)`

Plus les swapcontext de démarrage (main → A) et de fin (C → main) qui ne font pas partie d'un "tour" en tant que tel.

**Q5.3 — Oublier uc_link et les swapcontext finaux :**

Si `uc_link = NULL` et que les fonctions retournent normalement (sans swap explicite vers main), le comportement est **indéfini** — crash probable. Le processus n'a pas de contexte valide vers lequel sauter quand la fonction se termine.

**Q5.4 — Valeur finale prévisible :**

Oui, avec notre modèle coopératif (un seul thread noyau, pas de parallélisme réel), l'exécution est **totalement déterministe**. L'ordre des incréments est toujours A, B, C, A, B, C... donc `jeton` vaut toujours `3 * N_TOURS` à la fin.

Avec de vrais threads noyau (parallélisme), `jeton++` n'est pas atomique (c'est un read-modify-write), donc on aurait une **race condition** et le résultat serait imprévisible (et souvent faux). C'est exactement le problème des sections critiques que l'on verra au Cours 04.

</details>

---

➡️ Continuer avec `pingpong.c` — TP02 exercice 1
