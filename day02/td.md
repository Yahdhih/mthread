# TD — Exercices 3, 4 et 5

---

## Exercice 3 — Parallélisme vs Concurrence

**Situation A** : Machine avec 1 cœur CPU, 4 threads.
**Situation B** : Machine avec 4 cœurs CPU, 4 threads.
**Situation C** : Machine avec 2 cœurs CPU, 4 threads.

Pour chaque situation :
1. Y a-t-il parallélisme ? Concurrence ?
2. Combien de threads peuvent s'exécuter **simultanément** au sens strict ?
3. Dessine un diagramme temporel sur 8 unités de temps.

---

**Espace de réponse :**
```
Situation A :
 Concurrence 
  Max simultané : 1
  Diagramme :
  t=0 1 2 3 4 5 6 7
  C1: `[T1][T2][T3][T4][T1][T2][T3][T4]` 

Situation B :
  Parallélisme 
  Max simultané : 4
  Diagramme :
  t=0 1 2 3 4 5 6 7
  C1: `[T1][T1][T1][T1][T1][T1][T1][T1]`
      `[T2][T2][T2][T2][T2][T2][T2][T2]`
      `[T3][T3][T3][T3][T3][T3][T3][T3]`
      `[T4][T4][T4][T4][T4][T4][T4][T4]`

Situation C :
  Parallélisme et Concurrence 
  Max simultané : 2
  Diagramme :
  t=0 1 2 3 4 5 6 7
  C1: `[T1][T1][T1][T1][T2][T2][T2][T2]`
      `[T3][3][T3][T3][T4][T4][T4][T4]`
```

---

## Exercice 4 — Modèles de threads

**a)** Dans le modèle **1:1** (kernel threads), si T1 fait un appel système bloquant (`read()`), que se passe-t-il pour T2 ?

**b)** Dans le modèle **N:1** (user threads), même question.

**c)** En C avec `pthread`, quel modèle est utilisé sur Linux ?

**d)** Notre bibliothèque `mthread` utilisera le modèle N:1. Donne **deux avantages** et **deux inconvénients** de ce choix.

---

**Espace de réponse :**
```
a) Dans le modèle **1:1** (kernel thread), si T1 fait un appel système bloquant, T2 n'est pas effecté et continue son travail puisque les deux threads sont indépendants.

b) Dans le modèle **N:1**, lorsqu'un thread fait un appel système bloquant, tout l'ensemble des thread s'arrête sauf au cas ou faix un yield() et on laisse es autres threads travail.

c) Avec `pthread` le modèle utilisé est le modèle **1:1**

d) Avantages :
   1.très léger
   2.pas d'appel système
   Inconvénients :
   1.pas de vrai parallèlisme
   2.si un thread bloque, tous bloquent
```

---

## Exercice 5 — Question de réflexion

Un serveur web reçoit 1000 requêtes simultanées. Pour chaque requête, il lit un fichier sur disque (opération lente) puis envoie une réponse.

**a)** Pourquoi utiliser des threads plutôt qu'un seul processus séquentiel ?

**b)** Pourquoi un serveur réel utiliserait-il le modèle 1:1 (pthreads) plutôt que N:1 (mthread) dans ce cas ?

**c)** Quelle alternative aux threads existe pour gérer l'I/O sans créer un thread par connexion ? (Indice : `epoll`, `select`)

---

**Espace de réponse :**
```
a)

b)

c)
```

---

## Corrigé

<details>
<summary>Exercice 3</summary>

**A (1 cœur, 4 threads)** :
- Concurrence : oui. Parallélisme : non.
- Max simultané : 1.
- Diagramme : `[T1][T2][T3][T4][T1][T2][T3][T4]` (entrelacés)

**B (4 cœurs, 4 threads)** :
- Concurrence et parallélisme : oui.
- Max simultané : 4.
- Diagramme : 4 lignes simultanées.

**C (2 cœurs, 4 threads)** :
- Concurrence et parallélisme : oui.
- Max simultané : 2.
- Diagramme : 2 groupes de 2 qui s'alternent.

</details>

<details>
<summary>Exercice 4</summary>

a) Dans 1:1 : T1 est bloqué par le noyau mais T2 a son propre thread noyau → T2 continue de s'exécuter.

b) Dans N:1 : T1 bloque l'unique thread noyau → T2 est aussi bloqué. Toute la bibliothèque est gelée.

c) Linux utilise le modèle 1:1 avec `clone()`. Chaque `pthread` correspond à un LWP (Light Weight Process).

d) Avantages N:1 : création très rapide, pas d'appel système pour yield/switch. Inconvénients : pas de parallélisme réel sur multi-cœur, un `read()` bloquant gèle tout.

</details>
