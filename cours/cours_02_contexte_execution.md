# Cours 02 — Contexte d'exécution

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
La **pile d'appel** (call stack) stocke, pour chaque appel de fonction :
- Les **arguments** passés à la fonction
- L'**adresse de retour** (où revenir après le `return`)
- Les **variables locales**
- Le **rbp sauvegardé** (pour reconstruire la frame précédente)

```
État de la pile lors de f() appelant g() :

   rsp →  [ var locale g    ]  ← sommet de pile (adresse basse)
          [ rbp sauvegardé  ]
          [ adresse retour  ]  ← adresse dans f() après l'appel
          [ argument de g   ]
          [ var locale f    ]
          [ rbp sauvegardé  ]
          [ adresse retour  ]  ← adresse dans main() après l'appel
   rbp →  [ frame de main   ]
          ...
```

### Théorème 2.1 — Indépendance des piles
Chaque thread possède une pile **totalement indépendante**. Deux threads peuvent exécuter la **même fonction** simultanément sans conflit, car leurs variables locales sont sur des piles différentes.

**Preuve par l'exemple** :
```c
void compte(int n) {
    int i;           // variable locale → sur la pile du thread courant
    for (i = 0; i < n; i++) { ... }
}
// T1 et T2 appellent compte() simultanément → pas de conflit sur i
```

---

## 3. `ucontext_t` — L'API POSIX pour manipuler les contextes

C'est l'outil que nous utiliserons pour implémenter mthread.

### Définition 3.1 — `ucontext_t`
La structure `ucontext_t` (définie dans `<ucontext.h>`) représente un contexte d'exécution complet.

```c
#include <ucontext.h>

typedef struct ucontext_t {
    ucontext_t  *uc_link;    // contexte à activer quand celui-ci se termine
    stack_t      uc_stack;   // description de la pile associée
    mcontext_t   uc_mcontext; // registres CPU (rip, rsp, rbp, ...)
    sigset_t     uc_sigmask; // masque de signaux
} ucontext_t;
```

### Les 4 fonctions essentielles

#### `getcontext(ctx)` — Photographier le contexte actuel
```c
int getcontext(ucontext_t *ctx);
```
Sauvegarde le contexte **courant** dans `ctx`. Pense-y comme : "prends une photo de l'état du CPU maintenant."

#### `makecontext(ctx, func, argc, ...)` — Préparer un nouveau contexte
```c
void makecontext(ucontext_t *ctx, void (*func)(void), int argc, ...);
```
Configure `ctx` pour qu'il exécute `func` quand il sera activé. Doit être appelé **après** avoir alloué une pile dans `ctx->uc_stack`.

#### `swapcontext(old_ctx, new_ctx)` — Changer de contexte
```c
int swapcontext(ucontext_t *old_ctx, const ucontext_t *new_ctx);
```
1. Sauvegarde le contexte courant dans `old_ctx`
2. Restaure et exécute `new_ctx`
C'est le cœur d'un scheduler.

#### `setcontext(ctx)` — Activer un contexte (sans retour)
```c
int setcontext(const ucontext_t *ctx);
```
Restaure `ctx` sans sauvegarder le contexte courant. Ne revient jamais.

---

## 4. Comment fonctionne `swapcontext`

### Théorème 4.1 — Point de reprise
Quand on appelle `swapcontext(A, B)` :
- Le thread A est **suspendu** juste après l'appel
- Le thread B reprend là où **il** avait appelé `swapcontext` (ou là où `makecontext` a dit)
- La prochaine fois que A sera réactivé, il reprend la ligne **après** son `swapcontext`

```
Thread A                      Thread B
   ...
swapcontext(&A, &B) ──────→  reprend ici
   [suspendu]                    ...
   [suspendu]              swapcontext(&B, &A) ──→ reprend ici (après son swapcontext)
   reprend ici ←──────────
   ...
```

---

## 5. Allouer une pile pour un thread

Chaque thread a besoin d'une pile allouée explicitement :

```c
#define STACK_SIZE (64 * 1024)  // 64 Ko

ucontext_t ctx;
char *stack = malloc(STACK_SIZE);

getcontext(&ctx);
ctx.uc_stack.ss_sp   = stack;        // pointeur vers la pile
ctx.uc_stack.ss_size = STACK_SIZE;   // taille de la pile
ctx.uc_link          = NULL;         // que faire quand la fonction se termine
makecontext(&ctx, ma_fonction, 0);   // configurer pour exécuter ma_fonction
```

### Définition 5.1 — Stack overflow
Un **stack overflow** se produit quand la pile dépasse sa taille allouée. Symptôme : segfault mystérieux. Solution : allouer une pile suffisamment grande.

---

## 6. Schéma complet d'une commutation

```
État initial :
  main_ctx : rip=0x401a (dans main, après swapcontext)
  thread_ctx: rip=0x402c (dans worker, après swapcontext)
  CPU exécute : main

Appel swapcontext(&main_ctx, &thread_ctx) :
  1. CPU sauvegarde rip,rsp,rbp,... dans main_ctx
  2. CPU charge rip,rsp,rbp,... depuis thread_ctx
  3. CPU exécute worker() à partir de rip=0x402c

Plus tard, appel swapcontext(&thread_ctx, &main_ctx) dans worker :
  1. CPU sauvegarde dans thread_ctx
  2. CPU charge main_ctx
  3. main reprend après son swapcontext (rip=0x401a+1)
```

---

## Résumé du cours 02

| Concept              | À retenir                                              |
|----------------------|--------------------------------------------------------|
| Contexte             | Registres + pile = tout ce qu'il faut pour reprendre   |
| Context switch       | Sauvegarder l'un, charger l'autre                      |
| `ucontext_t`         | La structure POSIX qui représente un contexte          |
| `swapcontext`        | L'opération de commutation                             |
| Stack par thread     | Allouée manuellement, passée via `uc_stack`            |

---

**Précédent** : [Cours 01](cours_01_processus_threads.md) | **Suivant** : [Cours 03 — Cycle de vie](cours_03_cycle_vie.md)
