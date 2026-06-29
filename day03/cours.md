# Cours — Contexte d'exécution

---

## 1. Qu'est-ce que le contexte ?

### Définition 1.1 — Contexte d'exécution
Le **contexte d'exécution** d'un thread est l'ensemble des informations nécessaires pour **reprendre son exécution** là où elle s'est arrêtée :

```
Contexte d'un thread = {
    registres CPU  : rax, rbx, rcx, rdx, rsi, rdi, r8–r15
    pointeur pile  : rsp (stack pointer)
    pointeur base  : rbp (base pointer)
    compteur prog. : rip (instruction pointer / Program Counter)
    flags CPU      : rflags (résultat des comparaisons)
    pile           : les données empilées (variables locales, retours)
}
```

### Définition 1.2 — Commutation de contexte (Context Switch)
Une **commutation de contexte** est l'opération par laquelle le CPU :
1. **Sauvegarde** le contexte du thread courant en mémoire
2. **Charge** le contexte d'un autre thread depuis la mémoire
3. **Reprend** l'exécution du second thread

```
Avant switch :          Après switch :
Thread A : rip=0x401a   Thread A : [sauvegardé]
Thread B : [sauvegardé] Thread B : rip=0x402c  ← reprend ici
```

---

## 2. La pile (Stack)

### Définition 2.1 — Pile d'appel
La **pile d'appel** stocke, pour chaque appel de fonction :
- Les **arguments** passés à la fonction
- L'**adresse de retour** (où revenir après le `return`)
- Les **variables locales**
- Le **rbp sauvegardé**

```
État de la pile lors de f() appelant g() :

   rsp →  [ var locale g    ]  ← sommet (adresse basse)
          [ rbp sauvegardé  ]
          [ adresse retour  ]  ← dans f() après l'appel
          [ argument de g   ]
          [ var locale f    ]
          [ rbp sauvegardé  ]
          [ adresse retour  ]  ← dans main() après l'appel
```

### Théorème 2.1 — Indépendance des piles
Chaque thread possède une pile **totalement indépendante**. Deux threads peuvent exécuter la **même fonction** simultanément sans conflit car leurs variables locales sont sur des piles différentes.

---

## 3. `ucontext_t` — L'API POSIX pour manipuler les contextes

### Définition 3.1 — `ucontext_t`
```c
#include <ucontext.h>

typedef struct ucontext_t {
    ucontext_t  *uc_link;     // contexte à activer quand celui-ci se termine
    stack_t      uc_stack;    // pile associée
    mcontext_t   uc_mcontext; // registres CPU
    sigset_t     uc_sigmask;  // masque de signaux
} ucontext_t;
```

### `getcontext(ctx)` — Photographier le contexte actuel
```c
int getcontext(ucontext_t *ctx);
```
Sauvegarde le contexte **courant** dans `ctx`.

### `makecontext(ctx, func, argc, ...)` — Préparer un nouveau contexte
```c
void makecontext(ucontext_t *ctx, void (*func)(void), int argc, ...);
```
Configure `ctx` pour qu'il exécute `func` quand il sera activé.
⚠️ Doit être appelé **après** avoir alloué une pile dans `ctx->uc_stack`.

### `swapcontext(old_ctx, new_ctx)` — Changer de contexte
```c
int swapcontext(ucontext_t *old_ctx, const ucontext_t *new_ctx);
```
1. Sauvegarde le contexte courant dans `old_ctx`
2. Restaure et exécute `new_ctx`

### `setcontext(ctx)` — Activer sans retour
```c
int setcontext(const ucontext_t *ctx);
```
Restaure `ctx` **sans** sauvegarder le contexte courant. Ne revient jamais.

---

## 4. Comment fonctionne `swapcontext`

### Théorème 4.1 — Point de reprise
Quand on appelle `swapcontext(A, B)` :
- Le thread A est **suspendu** juste après l'appel
- Le thread B reprend là où **il** avait appelé `swapcontext` (ou à `makecontext`)
- La prochaine fois que A sera réactivé, il reprend la ligne **après** son `swapcontext`

```
Thread A                      Thread B
   ...
swapcontext(&A, &B) ──────→  reprend ici
   [suspendu]                    ...
   [suspendu]              swapcontext(&B, &A) ──→ Thread A reprend
   reprend ici ←──────────        (après son swapcontext)
   ...
```

---

## 5. Allouer une pile pour un thread

```c
#define STACK_SIZE (64 * 1024)  // 64 Ko

ucontext_t ctx;
char *stack = malloc(STACK_SIZE);

getcontext(&ctx);
ctx.uc_stack.ss_sp   = stack;        // pointeur vers la pile
ctx.uc_stack.ss_size = STACK_SIZE;   // taille
ctx.uc_link          = NULL;         // que faire quand la fonction retourne
makecontext(&ctx, ma_fonction, 0);
```

### Définition 5.1 — Stack overflow
Un **stack overflow** se produit quand la pile dépasse sa taille allouée → segfault.
Solution : allouer une pile suffisamment grande (64Ko est raisonnable).

---

## Résumé

| Concept        | À retenir                                          |
|----------------|----------------------------------------------------|
| Contexte       | Registres + pile = tout ce qu'il faut pour reprendre |
| Context switch | Sauvegarder l'un, charger l'autre                  |
| `ucontext_t`   | La structure POSIX qui représente un contexte      |
| `swapcontext`  | L'opération de commutation — retourne quand réactivé |
| `setcontext`   | Activation sans retour possible                    |
| Stack par thread | Allouée manuellement, passée via `uc_stack`      |
