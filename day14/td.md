# TD — Exercices 2, 3, 4, 5

---

## Exercice 2 — Deadlock par ordre de lock inversé

Voici deux threads :

```c
mthread_mutex_t A, B;

void *thread1(void *arg) {
    mthread_mutex_lock(&A);
    printf("T1 a pris A\n");
    mthread_yield();                // point d'entrelacement
    mthread_mutex_lock(&B);         // peut bloquer ici
    printf("T1 a pris B\n");
    mthread_mutex_unlock(&B);
    mthread_mutex_unlock(&A);
    return NULL;
}

void *thread2(void *arg) {
    mthread_mutex_lock(&B);
    printf("T2 a pris B\n");
    mthread_yield();
    mthread_mutex_lock(&A);         // peut bloquer ici
    printf("T2 a pris A\n");
    mthread_mutex_unlock(&A);
    mthread_mutex_unlock(&B);
    return NULL;
}
```

**a)** Identifie les 4 conditions de Coffman dans ce scénario.

**b)** Trace l'exécution entrelacée qui mène au deadlock (à partir du premier `yield`).

**c)** Propose une correction simple qui évite le deadlock sans changer la logique du programme.

---

**Espace de réponse :**
```
a)
1. Exclusion mutuelle :
2. Hold-and-wait :
3. Pas de préemption :
4. Attente circulaire :

b)
T1: lock(A)
T2: lock(B)
T1: yield → T2 s'exécute
T1: ...
T2: ...
Résultat :

c)
```

---

## Exercice 3 — Algorithme de Peterson

```c
int turn = 0;
int want[2] = {0, 0};

// Thread i (i=0, j=1) veut entrer dans la section critique
void lock_peterson(int i) {
    int j = 1 - i;
    want[i] = 1;
    turn = j;
    while (want[j] && turn == j) ;
}

void unlock_peterson(int i) {
    want[i] = 0;
}
```

**Scénario** : T0 et T1 veulent entrer en même temps.

**a)** Remplis la table d'états à chaque étape :

```
Étape | Thread | Instruction         | want[0] | want[1] | turn
------|--------|---------------------|---------|---------|-----
  1   |  T0    | want[0]=1           |         |         |
  2   |  T0    | turn=1              |         |         |
  3   |  T1    | want[1]=1           |         |         |
  4   |  T1    | turn=0              |         |         |
  5   |  T0    | while(want[1]&&...) |         |         |
  6   |  T1    | while(want[0]&&...) |         |         |
```

**b)** Lequel entre dans la section critique ? Pourquoi ?

**c)** Quand T0 sort et fait `unlock_peterson(0)`, que se passe-t-il pour T1 ?

---

**Espace de réponse :**
```
a) (compléter la table)

b)

c)
```

---

## Exercice 4 — Loi d'Amdahl

Formule : S(p) = 1 / (f + (1-f)/p)

**a)** f = 0.10 (10% séquentiel). Calcule S(p) pour p = 2, 4, 8, 16, ∞ :

| p   | S(p) |
|-----|------|
| 2   |      |
| 4   |      |
| 8   |      |
| 16  |      |
| ∞   |      |

**b)** f = 0.25 (25% séquentiel). Remplis le même tableau.

**c)** À partir de quel nombre de processeurs l'accélération est-elle "saturée" (moins de 10% de gain en doublant le nombre de processeurs) pour f=0.1 ?

**d)** Un développeur prétend que "plus de cœurs = toujours mieux". Que lui réponds-tu ?

---

**Espace de réponse :**
```
a)
p=2  : S =
p=4  : S =
p=8  : S =
p=16 : S =
p=∞  : S =

b)
p=2  : S =
p=4  : S =
...

c)

d)
```

---

## Exercice 5 — Dîner des Philosophes

**Contexte** : 5 philosophes sont assis autour d'une table ronde. Il y a 5 baguettes, une entre chaque paire. Un philosophe a besoin des **deux** baguettes adjacentes pour manger.

```
    [P0]
  🥢      🥢
[P4]        [P1]
  🥢      🥢
    [P3]-[P2]
```

**a)** Si chaque philosophe prend d'abord la baguette de gauche, puis celle de droite, un deadlock est-il possible ? Trace le scénario.

**b)** Identifie les 4 conditions de Coffman dans ce problème.

**c)** Propose une solution qui évite le deadlock par **ordre d'acquisition** : les philosophes 0-3 prennent gauche puis droite, le philosophe 4 prend droite puis gauche.
Pourquoi cela brise-t-il le deadlock ?

**d)** Propose une autre solution : utiliser un sémaphore `table` initialisé à 4 (au plus 4 philosophes assis simultanément).

---

**Espace de réponse :**
```
a)

b)
1.
2.
3.
4.

c)

d) sem_init(&table, 4);
   philosophe(i):
```

---

## Corrigé

<details>
<summary>Exercice 2</summary>

a) 
1. Exclusion mutuelle : A et B ne peuvent être détenus que par 1 thread
2. Hold-and-wait : T1 tient A et attend B ; T2 tient B et attend A
3. Pas de préemption : on ne peut pas retirer A de T1 de force
4. Attente circulaire : T1→B→T2→A→T1

b) T1 prend A, T2 prend B, T1 attend B (T2 le tient), T2 attend A (T1 le tient) → deadlock.

c) Correction : les deux threads prennent les locks dans le **même ordre** :
```c
// T2 aussi fait :
mthread_mutex_lock(&A);  // d'abord A
mthread_mutex_lock(&B);  // puis B
```

</details>

<details>
<summary>Exercice 3</summary>

a) Après étape 4 : want[0]=1, want[1]=1, turn=0.
Étape 5 : T0 teste `want[1] && turn==0` → vrai → T0 boucle.
Étape 6 : T1 teste `want[0] && turn==1` → faux (turn==0, pas 1) → T1 ENTRE.

b) T1 entre car le dernier à avoir écrit `turn` était T1 (`turn=0`), ce qui lui donne la priorité. T0 attend.

c) Quand T0 fait `want[0]=0`, T1 peut sortir de sa boucle while (si T1 était en attente). Ici T1 est déjà dans la SC, donc T0 peut entrer après que T1 sorte.

</details>

<details>
<summary>Exercice 4</summary>

a) f=0.1 :
- p=2 : 1/(0.1+0.45) = 1.82
- p=4 : 1/(0.1+0.225) = 3.08
- p=8 : 1/(0.1+0.1125) = 4.71
- p=16: 1/(0.1+0.05625) = 6.40
- p=∞ : 1/0.1 = 10

b) f=0.25 : p=∞ → 1/0.25 = 4. Limite absolue : 4×, même avec 1 million de cœurs.

c) Entre p=8 (4.71) et p=16 (6.40), le gain est 36%. Entre p=16 et p=32 : environ 22%. La saturation commence après p=16 pour f=0.1.

d) Non. La loi d'Amdahl montre que l'accélération est bornée par 1/f quelle que soit la puissance ajoutée. Optimiser la partie séquentielle est parfois plus efficace qu'ajouter des cœurs.

</details>

<details>
<summary>Exercice 5</summary>

a) Oui : chaque philosophe prend sa baguette gauche → tous les 5 tiennent une baguette → tous attendent la baguette droite (tenue par le voisin) → deadlock circulaire.

b) 
1. Exclusion mutuelle : une baguette = une ressource non partageable
2. Hold-and-wait : philosophe tient baguette gauche, attend la droite
3. Pas de préemption : on ne peut pas arracher une baguette
4. Attente circulaire : P0→baguette1→P1→baguette2→...→P4→baguette0→P0

c) P4 prend droite puis gauche → brise l'attente circulaire. Il y aura toujours au moins un philosophe capable de progresser (celui qui a les deux ou qui n'a pas de conflit de sens).

d) 
```c
sem_t table;
sem_init(&table, 4);  // au plus 4 assis

void philosophe(int i) {
    sem_wait(&table);         // s'asseoir
    prendre(gauche[i]);
    prendre(droite[i]);
    manger();
    poser(droite[i]);
    poser(gauche[i]);
    sem_post(&table);         // se lever
}
```
Si 4 philosophes au max sont assis, au moins l'un d'eux peut progresser.

</details>
