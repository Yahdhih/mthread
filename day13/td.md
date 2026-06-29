# TD — Exercice 1 : Race Condition

---

## Exercice 1 — Tracer l'entrelacement

Considère ce programme :

```c
int counter = 0;  // variable globale partagée

// T1 et T2 exécutent simultanément :
void increment(void) {
    counter = counter + 1;
}
```

En assembleur, `counter = counter + 1` donne :
```
LOAD  rax, [counter]
ADD   rax, 1
STORE [counter], rax
```

**a)** `counter` vaut 0. T1 et T2 exécutent `increment()` simultanément.
Complète l'entrelacement ci-dessous pour obtenir `counter == 1` au lieu de 2 :

```
       T1                         T2
LOAD  rax, [counter]  → rax=___
ADD   rax, 1          → rax=___
                           LOAD  rax, [counter]  → rax=___
                           ADD   rax, 1          → rax=___
STORE [counter], rax  → counter=___
                           STORE [counter], rax  → counter=___

Résultat final : counter = ___   (attendu : 2)
```

**b)** Y a-t-il un entrelacement qui donne le bon résultat (2) ? Si oui, trace-le.

**c)** La valeur `counter = 1` est-elle **toujours** le résultat en cas de race condition ?
(Indice : que se passe-t-il avec 3 threads ?)

**d)** Donne une correction qui évite la race condition en utilisant un mutex (pseudocode).

---

**Espace de réponse :**
```
a)
T1: LOAD  → rax=
T1: ADD   → rax=
T2: LOAD  → rax=
T2: ADD   → rax=
T1: STORE → counter=
T2: STORE → counter=
Résultat : counter=

b)

c)

d) // avec mutex :
```

---

## Corrigé

<details>
<summary>Exercice 1</summary>

**a)**
```
T1: LOAD  → rax=0
T1: ADD   → rax=1
T2: LOAD  → rax=0   ← T2 lit AVANT que T1 écrive → valeur obsolète
T2: ADD   → rax=1
T1: STORE → counter=1
T2: STORE → counter=1   ← écrase le résultat de T1
Résultat : counter=1  (FAUX, attendu : 2)
```

**b)** Oui — si les 3 instructions de T1 se déroulent sans interruption :
```
T1: LOAD→0, ADD→1, STORE→1
T2: LOAD→1, ADD→2, STORE→2
Résultat : 2  (correct)
```

**c)** Non. Avec 3 threads, on peut obtenir 1 (les 3 lisent 0, les 3 stockent 1) ou 2 (deux lisent 0, un lit 1). La valeur finale peut être 1, 2, ou 3 selon l'entrelacement.

**d)** 
```c
mthread_mutex_lock(&mu);
counter = counter + 1;
mthread_mutex_unlock(&mu);
```

</details>
