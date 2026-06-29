# TD02 — Exercices 2, 3, 4 : ucontext_t en profondeur

---

## Exercice 2 — getcontext + makecontext sans swap

Considère ce programme :

```c
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE (64 * 1024)

static ucontext_t ctx_a;

void fonction_a(void) {
    printf("Je suis dans fonction_a !\n");
}

int main(void) {
    char *pile = malloc(STACK_SIZE);

    getcontext(&ctx_a);
    ctx_a.uc_stack.ss_sp   = pile;
    ctx_a.uc_stack.ss_size = STACK_SIZE;
    ctx_a.uc_link          = NULL;

    makecontext(&ctx_a, fonction_a, 0);

    printf("Après makecontext, avant tout swap.\n");
    printf("Fin du main.\n");

    free(pile);
    return 0;
}
```

### Question 2.1

Qu'affiche ce programme ? Justifie.

```
[espace de réponse]
```

### Question 2.2

Quelle instruction faudrait-il ajouter (et où) pour que `fonction_a` s'exécute réellement ?

```
[espace de réponse]
```

### Question 2.3

Que se passe-t-il si `uc_link = NULL` et que `fonction_a` retourne sans qu'on ait défini un contexte de retour ? (Indice : essaie de compiler et d'exécuter en ajoutant le swap.)

```
[espace de réponse]
```

<details>
<summary>Corrigé exercice 2</summary>

**Q2.1 — Sortie du programme :**

```
Après makecontext, avant tout swap.
Fin du main.
```

`makecontext` ne lance pas la fonction. Il *configure* le contexte `ctx_a` pour qu'il démarre dans `fonction_a` quand on fera un `swapcontext` ou `setcontext` vers lui. Sans ce swap, `fonction_a` n'est jamais appelée.

**Q2.2 — Instruction à ajouter :**

Après le `makecontext`, ajouter :

```c
ucontext_t ctx_main;
swapcontext(&ctx_main, &ctx_a);
```

Cela sauvegarde l'état de `main` dans `ctx_main` et saute dans `ctx_a` (qui exécutera `fonction_a`).

**Q2.3 — Si uc_link = NULL et que la fonction retourne :**

Quand `fonction_a` retourne et que `uc_link == NULL`, le comportement est **indéfini**. En pratique, sur Linux/macOS, le processus se termine brutalement (souvent SIGSEGV ou abort) car il n'y a pas de contexte valide vers lequel retourner. C'est pour ça qu'on utilise toujours `uc_link` ou qu'on appelle `mthread_exit` à la fin d'un contexte.

</details>

---

## Exercice 3 — Ping-Pong à deux contextes : tracer l'exécution

Voici un programme complet. Il compile et fonctionne correctement.

```c
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE (64 * 1024)

static ucontext_t ctx_main, ctx_ping, ctx_pong;

void pong_func(void) {
    printf("Pong 1\n");           // (D)
    swapcontext(&ctx_pong, &ctx_ping);
    printf("Pong 2\n");           // (F)
    swapcontext(&ctx_pong, &ctx_main);
}

void ping_func(void) {
    printf("Ping 1\n");           // (B)
    swapcontext(&ctx_ping, &ctx_pong);
    printf("Ping 2\n");           // (E)
    swapcontext(&ctx_ping, &ctx_pong);
    // (G) — ping_func retourne, uc_link pointe vers ctx_main
}

int main(void) {
    char *stack_ping = malloc(STACK_SIZE);
    char *stack_pong = malloc(STACK_SIZE);

    getcontext(&ctx_ping);
    ctx_ping.uc_stack.ss_sp   = stack_ping;
    ctx_ping.uc_stack.ss_size = STACK_SIZE;
    ctx_ping.uc_link          = &ctx_main;
    makecontext(&ctx_ping, ping_func, 0);

    getcontext(&ctx_pong);
    ctx_pong.uc_stack.ss_sp   = stack_pong;
    ctx_pong.uc_stack.ss_size = STACK_SIZE;
    ctx_pong.uc_link          = &ctx_main;
    makecontext(&ctx_pong, pong_func, 0);

    printf("Main début\n");        // (A)
    swapcontext(&ctx_main, &ctx_ping);
    printf("Main fin\n");          // (H)

    free(stack_ping);
    free(stack_pong);
    return 0;
}
```

### Question 3.1

Trace l'exécution **pas à pas**, en indiquant à chaque étape :
- quel contexte s'exécute
- quelle instruction s'exécute (utilise les lettres A, B, C... marquées dans le code)
- quel est l'effet du `swapcontext` (qui sauvegarde quoi, qui reprend quoi)

```
Étape 1 : contexte = main,      instruction = (A) → affiche "Main début"
Étape 2 : contexte = main,      instruction = swapcontext(&ctx_main, &ctx_ping)
           → sauvegarde main dans ctx_main, restaure ctx_ping
Étape 3 : ...

[continue ici]
```

### Question 3.2

Quelle est la sortie complète du programme ?

```
[espace de réponse]
```

### Question 3.3

Au total, combien de fois `swapcontext` est-il appelé ? Liste-les.

```
[espace de réponse]
```

<details>
<summary>Corrigé exercice 3</summary>

**Q3.1 — Trace pas à pas :**

```
Étape 1  : main      → (A) affiche "Main début"
Étape 2  : main      → swapcontext(&ctx_main, &ctx_ping)
            sauve main dans ctx_main, démarre ctx_ping (ping_func)
Étape 3  : ping_func → (B) affiche "Ping 1"
Étape 4  : ping_func → swapcontext(&ctx_ping, &ctx_pong)
            sauve ping dans ctx_ping, démarre ctx_pong (pong_func)
Étape 5  : pong_func → (D) affiche "Pong 1"
Étape 6  : pong_func → swapcontext(&ctx_pong, &ctx_ping)
            sauve pong dans ctx_pong, reprend ctx_ping juste après son swapcontext
Étape 7  : ping_func → (E) affiche "Ping 2"
Étape 8  : ping_func → swapcontext(&ctx_ping, &ctx_pong)
            sauve ping dans ctx_ping, reprend ctx_pong juste après son swapcontext
Étape 9  : pong_func → (F) affiche "Pong 2"
Étape 10 : pong_func → swapcontext(&ctx_pong, &ctx_main)
            sauve pong dans ctx_pong, reprend ctx_main juste après son swapcontext
Étape 11 : main      → (H) affiche "Main fin"
Étape 12 : main      → free, return 0
```

Note : `ping_func` se termine en retournant, ce qui déclenche `uc_link = &ctx_main`. Mais ici on ne passe jamais par ce chemin car on swap explicitement vers `ctx_main` depuis `pong_func`.

**Q3.2 — Sortie :**

```
Main début
Ping 1
Pong 1
Ping 2
Pong 2
Main fin
```

**Q3.3 — Nombre de swapcontext :**

5 appels :
1. `swapcontext(&ctx_main, &ctx_ping)` dans main
2. `swapcontext(&ctx_ping, &ctx_pong)` dans ping (premier)
3. `swapcontext(&ctx_pong, &ctx_ping)` dans pong (premier)
4. `swapcontext(&ctx_ping, &ctx_pong)` dans ping (second)
5. `swapcontext(&ctx_pong, &ctx_main)` dans pong (second)

</details>

---

## Exercice 4 — Le rôle de uc_link

### Question 4.1

Qu'est-ce que `uc_link` dans une `ucontext_t` ? Quand est-il utilisé ?

```
[espace de réponse]
```

### Question 4.2

Considère ce code :

```c
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE (64 * 1024)

static ucontext_t ctx_main, ctx_worker;

void worker(void) {
    printf("Worker: début\n");
    printf("Worker: fin\n");
    // worker retourne ici — que se passe-t-il ?
}

int main(void) {
    char *stack = malloc(STACK_SIZE);

    getcontext(&ctx_worker);
    ctx_worker.uc_stack.ss_sp   = stack;
    ctx_worker.uc_stack.ss_size = STACK_SIZE;
    ctx_worker.uc_link          = &ctx_main;  // ← ici
    makecontext(&ctx_worker, worker, 0);

    printf("Main: avant swap\n");
    swapcontext(&ctx_main, &ctx_worker);
    printf("Main: après retour de worker\n");

    free(stack);
    return 0;
}
```

Trace l'exécution et explique ce qui se passe quand `worker` retourne.

```
[espace de réponse]
```

### Question 4.3

Si on avait mis `ctx_worker.uc_link = NULL` à la place, qu'est-ce qui se passerait quand `worker` retourne ?

```
[espace de réponse]
```

### Question 4.4

Dans une bibliothèque de threads (comme mthread), est-il judicieux d'utiliser `uc_link` pour revenir au scheduler ? Quel est le problème potentiel ?

```
[espace de réponse]
```

<details>
<summary>Corrigé exercice 4</summary>

**Q4.1 — uc_link :**

`uc_link` est un pointeur vers un `ucontext_t`. Quand la fonction associée à un contexte **retourne** (atteint son `return` ou la fin du corps), le système restaure automatiquement le contexte pointé par `uc_link`. C'est comme un "contexte de retour implicite".

Il n'est utilisé que lorsque la fonction se termine normalement. Un `swapcontext` ou `setcontext` explicite ignore `uc_link`.

**Q4.2 — Trace avec uc_link = &ctx_main :**

```
Étape 1 : main → affiche "Main: avant swap"
Étape 2 : main → swapcontext(&ctx_main, &ctx_worker)
           sauve main dans ctx_main, démarre ctx_worker
Étape 3 : worker → affiche "Worker: début"
Étape 4 : worker → affiche "Worker: fin"
Étape 5 : worker retourne → uc_link = &ctx_main → système restaure ctx_main
Étape 6 : main reprend juste après le swapcontext
           → affiche "Main: après retour de worker"
Étape 7 : main → free, return 0
```

Sortie :
```
Main: avant swap
Worker: début
Worker: fin
Main: après retour de worker
```

**Q4.3 — Si uc_link = NULL :**

Quand `worker` retourne et que `uc_link == NULL`, le comportement est indéfini. En pratique : crash, SIGSEGV, ou terminaison abrupte du processus. Il n'y a aucun contexte valide vers lequel sauter.

**Q4.4 — uc_link dans mthread :**

En théorie, on pourrait mettre `uc_link = &scheduler_ctx` pour que chaque thread revienne automatiquement au scheduler quand il se termine. Mais c'est **dangereux** : si le thread appelle `mthread_exit` (ce qu'il devrait toujours faire pour passer `retval` et notifier les joiners), `mthread_exit` appelle lui-même `setcontext(scheduler)`. Si la fonction retourne *sans* appeler `mthread_exit`, `uc_link` rattrape la situation.

Le problème : `uc_link` ne permet pas de passer des arguments (comme `retval`). La bonne pratique est d'envelopper l'appel dans un `thread_entry` :

```c
static void thread_entry(void) {
    void *ret = current->func(current->arg);
    mthread_exit(ret);  // jamais de return implicite
}
```

Et de mettre `uc_link = NULL` (ou le contexte scheduler), mais en comptant sur `mthread_exit` pour la transition. C'est ce qu'on fera dans mthread.

</details>

---

➡️ Demain : `day05/` — TD02 fin + TP02 début : pingpong.c
