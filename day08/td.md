# TD03 — Exercice 1 : Tracer un scénario de cycle de vie

**Durée estimée : 20 min**

---

## Exercice 1 — Scénario : main crée T1, T1 yield, T1 exit, main join T1

Voici le diagramme des états :

```
         mthread_create()
              │
              ▼
         [  READY  ] ←─────────────────────────────┐
              │                                     │
    scheduler choisit                         mthread_yield()
              │                                     │
              ▼                                     │
         [ RUNNING ] ──── mthread_yield() ──────────┘
              │
              ├──── mthread_exit() ────────────► [ ZOMBIE ]
              │                                      │
              └──── mthread_join() (bloquant) ──► [ BLOCKED ]
                         ↑ (si cible non zombie)      │
                         └── cible exit → réveille ───┘
```

Voici le programme qui génère le scénario à tracer :

```c
#include "mthread.h"
#include <stdio.h>

void *T1_func(void *arg) {
    (void)arg;
    printf("T1: je démarre\n");          // (C)
    mthread_yield();                      // (D)
    printf("T1: après yield\n");          // (E)
    mthread_exit((void *)42);             // (F)
    return NULL; // jamais atteint
}

int main(void) {
    mthread_t t1;
    printf("Main: je crée T1\n");         // (A)
    mthread_create(&t1, T1_func, NULL);   // (B)
    mthread_yield();                      // (G) — déclenche le scheduler
    printf("Main: avant join\n");         // (H)
    void *retval;
    mthread_join(t1, &retval);            // (I)
    printf("Main: retval = %ld\n", (long)retval); // (J)
    return 0;
}
```

### Question 1.1

Trace l'exécution **pas à pas**, en remplissant ce tableau :

| Étape | Contexte actif | Instruction | État de T1 | File READY |
|-------|---------------|-------------|------------|------------|
| 1     | main          | (A)         | NONEXISTANT | vide      |
| 2     | main          | (B) create  | → READY    | [T1]       |
| 3     | ?             | ?           | ?          | ?          |
| 4     | ?             | ?           | ?          | ?          |
| 5     | ?             | ?           | ?          | ?          |
| 6     | ?             | ?           | ?          | ?          |
| 7     | ?             | ?           | ?          | ?          |
| 8     | ?             | ?           | ?          | ?          |
| 9     | ?             | ?           | ?          | ?          |
| 10    | ?             | ?           | ?          | ?          |

```
[complète le tableau ici]
```

### Question 1.2

À l'étape (G), main appelle `mthread_yield()`. Pourquoi le scheduler démarre-t-il T1 et non main lui-même ?

```
[espace de réponse]
```

### Question 1.3

À l'étape (D), T1 appelle `mthread_yield()`. Que se passe-t-il dans la file READY ?

```
[espace de réponse]
```

### Question 1.4

À l'étape (I), main appelle `mthread_join(t1, &retval)`. À ce moment, T1 est-il ZOMBIE ou encore RUNNING ? Justifie en te basant sur la trace.

```
[espace de réponse]
```

<details>
<summary>Corrigé exercice 1</summary>

**Q1.1 — Tableau complet :**

| Étape | Contexte actif | Instruction | État de T1 | File READY |
|-------|---------------|-------------|------------|------------|
| 1  | main      | (A) printf              | NONEXISTANT | vide        |
| 2  | main      | (B) mthread_create      | READY       | [T1]        |
| 3  | main      | (G) mthread_yield()     | READY       | [T1, main]  |
| 4  | scheduler | dequeue → T1            | RUNNING     | [main]      |
| 5  | T1        | (C) printf "je démarre" | RUNNING     | [main]      |
| 6  | T1        | (D) mthread_yield()     | READY       | [main, T1]  |
| 7  | scheduler | dequeue → main          | READY       | [T1]        |
| 8  | main      | (H) printf              | READY       | [T1]        |
| 9  | main      | (I) mthread_join(t1)    | READY→RUNNING→ZOMBIE | [T1 → en cours] |
| *  | scheduler | dequeue → T1            | RUNNING     | vide        |
| *  | T1        | (E) printf "après yield"| RUNNING     | vide        |
| *  | T1        | (F) mthread_exit(42)    | ZOMBIE      | [main]      |
| *  | scheduler | dequeue → main          | ZOMBIE      | vide        |
| *  | main      | (J) printf retval=42    | ZOMBIE      | vide        |

Note : à l'étape (I), si T1 n'est pas encore ZOMBIE, main se bloque (BLOCKED) et rejoint la file quand T1 appelle exit.

**Q1.2 — Pourquoi T1 et pas main :**

Après `mthread_yield()`, main s'ajoute **en queue** de la file READY. T1 était déjà en tête (il a été créé avant). Le scheduler prend la tête de la file (FIFO), donc il choisit T1. Main attendra son tour.

**Q1.3 — mthread_yield() dans T1 :**

T1 passe en état READY et est ajouté **en queue** de la file. La file contenait [main] (main attendait depuis son propre yield). Elle devient [main, T1]. Le scheduler reprend et choisit main (tête de file).

**Q1.4 — État de T1 à l'étape join :**

D'après la trace : à l'étape (I), T1 est encore en état READY (il n'a pas encore exécuté (E) et (F)). Donc `mthread_join` prend le chemin "cas 2" : main se bloque (BLOCKED), stocke `t1->joiner = main`. Le scheduler exécute T1, qui fait exit, qui réveille main. Main se retrouve en READY, puis RUNNING, puis affiche le retval.

</details>

---

➡️ Demain : `day09/` — TD03 : exercices 2 à 5
