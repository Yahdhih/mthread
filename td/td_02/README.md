# TD 02 — Contexte d'exécution

> Exercices sur papier. Durée estimée : 1h30.

---

## Exercice 1 — La pile d'appel

Considère ce programme :

```c
int add(int a, int b) {
    int result = a + b;   // (3)
    return result;
}

int multiply(int x) {
    int y = x * 2;        // (2)
    return add(y, 10);
}

int main() {
    int r = multiply(5);  // (1)
    return 0;
}
```

a) Dessine l'état de la pile au moment où l'exécution atteint le point **(3)** (dans `add`). Indique pour chaque frame : les variables locales, l'adresse de retour, et le rbp sauvegardé.

b) Dans quel ordre les frames sont-elles empilées ?

c) Que se passe-t-il dans la pile quand `add` retourne ?

d) Si `result` est déclaré dans `add`, est-il accessible depuis `multiply` après que `add` ait retourné ? Pourquoi ?

---

## Exercice 2 — Contexte d'exécution

On a deux threads T1 et T2. On note `[T1: rip=0x40A, rsp=0x7F00]` l'état de T1.

**État initial :**
- T1 : rip=0x401A, rsp=0x7F00, rbp=0x7F80 (en cours d'exécution)
- T2 : rip=0x402C, rsp=0x6F00, rbp=0x6F80 (contexte sauvegardé)

a) T1 appelle `swapcontext(&ctx_T1, &ctx_T2)`. Décris ce qui se passe, étape par étape.

b) Après le swap, quel thread s'exécute ? À quelle adresse ?

c) T2 exécute quelques instructions puis appelle `swapcontext(&ctx_T2, &ctx_T1)`. Où reprend T1 ?

d) Pourquoi est-il important que chaque thread ait sa propre pile ?

---

## Exercice 3 — `ucontext_t` et `makecontext`

Voici un squelette de code :

```c
ucontext_t ctx_main, ctx_worker;
char stack[64 * 1024];

void worker() {
    printf("Je suis le worker\n");
    swapcontext(&ctx_worker, &ctx_main);   // (*)
    printf("Worker : deuxième passage\n");
}

int main() {
    getcontext(&ctx_worker);
    ctx_worker.uc_stack.ss_sp   = stack;
    ctx_worker.uc_stack.ss_size = sizeof(stack);
    ctx_worker.uc_link          = NULL;
    makecontext(&ctx_worker, worker, 0);

    printf("Avant le premier swap\n");
    swapcontext(&ctx_main, &ctx_worker);   // (A)
    printf("Retour dans main\n");          // (B)
    swapcontext(&ctx_main, &ctx_worker);   // (C)
    printf("Fin\n");
    return 0;
}
```

a) Donne l'ordre exact des `printf` affichés.

b) Que se passerait-il si `worker` ne faisait pas `swapcontext` au point `(*)` et retournait directement ?

c) Que contient `ctx_main` après l'appel en `(A)` ?

d) `uc_link = NULL` signifie que quand `worker` retourne, il n'y a pas de contexte de repli. Que se passe-t-il si `worker` retourne sans `swapcontext` ?

---

## Exercice 4 — Allocation de pile

a) Une pile de thread de 64 Ko peut contenir combien de frames de 100 octets environ (variable locale + adresse retour + rbp) ?

b) Qu'est-ce qu'un stack overflow dans le contexte des threads ? Comment le détecter ?

c) On veut créer 10 000 threads avec des piles de 8 Mo chacun (la valeur par défaut de pthread sur Linux). Combien de mémoire virtuelle faut-il ? Est-ce un problème ?

d) Pour notre bibliothèque `mthread`, quelle taille de pile recommanderais-tu ? Justifie.

---

## Exercice 5 — Trace d'exécution

Deux threads T1 et T2 alternent via yield. T1 exécute `A1 → yield → A2`, T2 exécute `B1 → yield → B2`.

```
Départ : T1 est en RUNNING, T2 est en READY
```

Trace complète tous les changements d'état et les appels à `swapcontext`. Pour chaque étape, indique :
- Quel thread est RUNNING
- Quel thread est READY
- Quel `swapcontext` est appelé

---

## Corrigé

<details>
<summary>Exercice 3 — Ordre des printf</summary>

```
Avant le premier swap
Je suis le worker
Retour dans main
Worker : deuxième passage
Fin
```

Explication :
1. main affiche "Avant..."
2. (A) → swapcontext sauvegarde main, active worker
3. worker affiche "Je suis le worker"
4. (*) → swapcontext sauvegarde worker, réactive main
5. main reprend après (A), affiche "Retour dans main"
6. (C) → swapcontext réactive worker
7. worker reprend après (*), affiche "Worker : deuxième passage"
8. worker retourne → uc_link = NULL → comportement indéfini (en pratique, segfault ou fin)

</details>

<details>
<summary>Exercice 5 — Trace</summary>

```
État 0 : T1 RUNNING, T2 READY
  T1 exécute A1
  T1 appelle yield() → swapcontext(&T1.ctx, &scheduler.ctx)

État 1 : T1 READY, T2 devient RUNNING
  scheduler choisit T2
  swapcontext(&scheduler.ctx, &T2.ctx)
  T2 exécute B1
  T2 appelle yield() → swapcontext(&T2.ctx, &scheduler.ctx)

État 2 : T1 RUNNING, T2 READY
  scheduler choisit T1
  swapcontext(&scheduler.ctx, &T1.ctx)
  T1 reprend après son yield, exécute A2
  T1 se termine

État 3 : T2 RUNNING
  scheduler choisit T2
  T2 reprend après son yield, exécute B2
  T2 se termine
```

</details>
