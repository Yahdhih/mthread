# Cours 01 — Processus et Threads : Fondements

---

## 1. Le processus

### Définition 1.1 — Processus
Un **processus** est un programme en cours d'exécution. Il possède :
- Un espace d'adressage **privé** (code, heap, stack, données statiques)
- Un identifiant unique : le **PID** (Process ID)
- Des ressources propres : fichiers ouverts, signaux, timer

```
Mémoire d'un processus :
┌──────────────┐ adresse haute
│    Stack     │ ← grandit vers le bas (variables locales, appels)
│      ↓       │
│              │
│      ↑       │
│    Heap      │ ← grandit vers le haut (malloc)
├──────────────┤
│  Données     │ ← variables globales/statiques initialisées
│  BSS         │ ← variables globales non initialisées
├──────────────┤
│   Code       │ ← instructions du programme (.text)
└──────────────┘ adresse basse (0x0)
```

### Définition 1.2 — Espace d'adressage virtuel
Le système d'exploitation donne à chaque processus l'**illusion** qu'il possède toute la mémoire. La MMU (Memory Management Unit) traduit les adresses virtuelles en adresses physiques.

**Conséquence** : deux processus avec le même pointeur `0x7fff1234` pointent vers deux zones mémoire physiques différentes.

---

## 2. Le thread

### Définition 2.1 — Thread (fil d'exécution)
Un **thread** est une unité d'exécution **à l'intérieur** d'un processus. Plusieurs threads coexistent dans le même espace d'adressage.

Chaque thread possède **en propre** :
- Une **pile** (stack) dédiée
- Un **Program Counter** (PC) — l'adresse de la prochaine instruction
- Des **registres** du CPU (rax, rbx, rsp, rbp…)
- Un **état** (running, ready, blocked…)

Tous les threads d'un processus **partagent** :
- Le segment de code (.text)
- Le heap (malloc/free)
- Les variables globales et statiques
- Les descripteurs de fichiers

```
Un processus avec 3 threads :
┌─────────────────────────────────────┐
│           Espace d'adressage        │
│  ┌────────┐ ┌────────┐ ┌────────┐  │
│  │Stack T1│ │Stack T2│ │Stack T3│  │
│  └────────┘ └────────┘ └────────┘  │
│                                     │
│            Heap partagé             │
│            Code partagé             │
│            Données partagées        │
└─────────────────────────────────────┘
   T1: PC=0x401a  T2: PC=0x402c  T3: PC=0x4038
```

### Définition 2.2 — Thread principal (main thread)
Quand un programme démarre, il possède **un seul thread** : le thread principal. C'est lui qui exécute `main()`. Tous les autres threads sont créés explicitement.

---

## 3. Processus vs Thread

### Théorème 3.1 — Isolation des processus
Deux processus **ne peuvent pas** accéder directement à la mémoire l'un de l'autre. Toute communication passe par le noyau (pipes, sockets, mémoire partagée explicite).

**Corollaire** : un crash dans un processus n'affecte pas les autres.

### Théorème 3.2 — Partage de mémoire entre threads
Tous les threads d'un même processus voient la **même mémoire**. Une variable globale modifiée par T1 est immédiatement visible par T2.

**Corollaire** : c'est à la fois la force (communication rapide) et le danger (corruption si accès concurrent non contrôlé) des threads.

| Critère              | Processus           | Thread             |
|----------------------|---------------------|--------------------|
| Espace mémoire       | Privé               | Partagé            |
| Coût de création     | Élevé (fork)        | Faible             |
| Communication        | IPC (lente)         | Mémoire (rapide)   |
| Isolation            | Forte               | Aucune             |
| Crash                | Isolé               | Tue tout le proc.  |

---

## 4. Pourquoi les threads ?

### Définition 4.1 — Parallélisme
Le **parallélisme** consiste à exécuter plusieurs tâches **simultanément** sur plusieurs cœurs CPU.

### Définition 4.2 — Concurrence
La **concurrence** consiste à gérer plusieurs tâches qui **progressent** en même temps (pas nécessairement au même instant). Sur un seul cœur, les threads s'alternent.

```
Parallélisme (2 cœurs) :
Cœur 1 : [T1====][T1====][T1====]
Cœur 2 : [T2====][T2====][T2====]

Concurrence (1 cœur) :
Cœur 1 : [T1==][T2==][T1==][T2==][T3=]
```

### Cas d'usage typiques
1. **Serveur web** : un thread par connexion client
2. **Calcul parallèle** : diviser un tableau sur N threads
3. **I/O non bloquant** : un thread attend le disque, un autre calcule
4. **UI responsive** : thread d'affichage séparé du thread de calcul

---

## 5. Modèles d'implémentation des threads

### Définition 5.1 — Threads noyau (1:1)
Chaque thread utilisateur correspond à un thread noyau. Le scheduler du noyau les gère directement.
- Utilisé par : Linux (`pthread`), Windows
- Avantage : vrai parallélisme sur plusieurs cœurs
- Inconvénient : appel système coûteux pour chaque opération

### Définition 5.2 — Threads utilisateur (N:1)
Plusieurs threads utilisateur sont multiplexés sur **un seul** thread noyau. La bibliothèque gère le scheduling.
- Avantage : très léger, pas d'appel système
- Inconvénient : pas de vrai parallélisme ; si un thread bloque, tous bloquent

### Définition 5.3 — Modèle hybride (M:N)
M threads utilisateur sur N threads noyau (M ≥ N).
- Utilisé par : Go (goroutines), Erlang

> **Notre bibliothèque `mthread` implémentera le modèle N:1** (threads utilisateur purs). C'est le plus simple à implémenter et il suffit pour comprendre tous les concepts.

---

## Résumé du cours 01

| Concept               | À retenir                                              |
|-----------------------|--------------------------------------------------------|
| Processus             | Programme en exécution, mémoire privée                 |
| Thread                | Flux d'exécution dans un processus, mémoire partagée   |
| Stack                 | Chaque thread a sa propre pile                         |
| Parallélisme          | Vraie simultanéité (multi-cœurs)                       |
| Concurrence           | Progression entrelacée (peut être mono-cœur)           |
| Modèle N:1            | Notre cible pour mthread                               |

---

**Suivant** : [Cours 02 — Contexte d'exécution](cours_02_contexte_execution.md)
