# TD — Exercice 1 : Diagrammes de Gantt

---

## Exercice 1 — FIFO vs Round-Robin

**Threads** (arrivée et durée d'exécution en unités de temps) :

| Thread | Arrive à | Durée totale |
|--------|----------|-------------|
| T1     | t=0      | 6           |
| T2     | t=1      | 4           |
| T3     | t=2      | 2           |
| T4     | t=3      | 3           |

**a) FIFO** (sans préemption — on attend qu'un thread finisse avant de passer au suivant) :

Dessine le diagramme de Gantt de t=0 à t=15 :
```
T1 : |___|___|___|___|___|___|
T2 :                         |___|___|___|___|
T3 :
T4 :
     0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
```

Calcule :
- Temps de complétion de T1 : ___
- Temps de complétion de T2 : ___
- Temps de complétion de T3 : ___
- Temps de complétion de T4 : ___
- Temps de complétion moyen : ___

**b) Round-Robin** (quantum = 2 unités) :

Les threads arrivent dans la file READY à leur instant d'arrivée.

Dessine le diagramme de Gantt :
```
t=0  T1 arrive, T1 s'exécute 2u
t=2  T2 arrive (dans la file), T1 yield → file=[T2, T1]
     T2 s'exécute 2u
...
```

Calcule les mêmes métriques et compare avec FIFO.

**c)** Pour quelle métrique Round-Robin est-il meilleur que FIFO ?

---

**Espace de réponse :**
```
a) Gantt FIFO :
T1 : [0-6]
T2 : [6-10]
T3 : [10-12]
T4 : [12-15]

Complétions :
T1 = 6, T2 = 10, T3 = 12, T4 = 15
Moyen = (6+10+12+15)/4 = ___

b) Gantt Round-Robin :
[Construire étape par étape]

c)
```

---

## Corrigé

<details>
<summary>Exercice 1</summary>

a) FIFO :
- T1 : [0-6], fin=6
- T2 : [6-10], fin=10
- T3 : [10-12], fin=12
- T4 : [12-15], fin=15
- Moyen : (6+10+12+15)/4 = 43/4 = **10.75**

b) Round-Robin (quantum=2) :
```
t=0  T1 s'exécute [0-2]
t=2  T2 arrive. File=[T2, T1]. T2 s'exécute [2-4]
t=4  T3 arrive. File=[T1, T3, T2]. T1 s'exécute [4-6]
t=6  T1 finit. T4 arrive. File=[T3, T2, T4]
     T3 s'exécute [6-8] → T3 finit (2u)
t=8  File=[T2, T4]. T2 s'exécute [8-10] → T2 finit (reste 2u)
t=10 File=[T4]. T4 s'exécute [10-12] → T4 finit (reste 1u? non, 3u-0=3u déjà 0u utilisé avant)
```

Recalcul RR propre :
```
t=0-2: T1(6→4u restant)
t=2-4: T2(4→2u restant)  [T2 arrive à t=1, file=[T2] à t=2]
t=4-6: T3 arrive à t=2, file à t=4=[T1,T3,T2]; T1(4→2u)
t=6-8: T3(2→0) fin à t=8  [T4 arrive à t=3, file=[T2,T4]]
t=8-10: T2(2→0) fin à t=10
t=10-12: T4(3→1)
t=12-14: T1(2→0) fin à t=14
t=14-15: T4(1→0) fin à t=15
```

Complétions RR : T3=8, T2=10, T1=14, T4=15
Moyen : (8+10+14+15)/4 = 47/4 = **11.75**

c) FIFO est meilleur en temps de complétion moyen ici (10.75 < 11.75) car les threads courts arrivent tôt et attendent peu. RR est meilleur pour le **temps de réponse** (première exécution) et pour l'**interactivité** : T2 et T3 commencent plus tôt en RR.

</details>
