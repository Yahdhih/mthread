# TD — Exercices 1 et 2

---

## Exercice 1 — Vrai ou Faux (justifier en 1 phrase chacun)

1. Deux processus distincts peuvent lire la même adresse mémoire virtuelle `0x7fff1234` et obtenir la même valeur.

2. Si T1 et T2 sont deux threads du même processus, une variable **locale** de T1 est visible par T2.

3. Un crash dans le thread T1 d'un processus P peut tuer tous les threads de P.

4. La création d'un thread est moins coûteuse que la création d'un processus.

5. Dans le modèle N:1, si un thread fait un appel système bloquant (ex: `read()`), tous les threads du processus sont bloqués.

---

**Espace de réponse :**
```
1.vrai
2.faux
3.vrai
4.vrai
5.faux
```

---

## Exercice 2 — Schéma mémoire

Considère ce programme :

```c
int globale = 42;           // (A)

void *worker(void *arg) {
    int locale = 10;        // (B)
    int *heap  = malloc(4); // (C) — pointeur
    *heap = 99;             //       valeur pointée
    return NULL;
}

int main() {
    int main_locale = 5;    // (D)
    // crée 2 threads qui exécutent worker()
}
```

**a)** Remplis le tableau :

| Variable       | Segment mémoire | Partagée entre les 2 threads ? |
|----------------|-----------------|-------------------------------|
| A (globale)    | .data (données BSS)  |oui                            |
| B (locale)     |  stack               |non                            |
| C (pointeur)   |  stack               |non                            |
| *heap (valeur) |  heap                |non                            |
| D (main_locale)|  stack               |non                            |

**NB :** La Heap est partagés entre les threads

**b)** Les deux threads exécutent `worker()` simultanément. Peuvent-ils avoir deux valeurs **différentes** pour `locale` ? Pourquoi ?

**c)** Si T1 fait `globale = 100` et que T2 lit ensuite `globale`, quelle valeur lit-il ?

---

**Espace de réponse :**
```
a) voir le tableau
A →
B →
C →
*heap →
D →

b) Non `locale` a la même valeur pour les deux threads, il peut avoir deux valeur différent si on crée deux `worker()` différent, mais comment on a le meme `worker()`, la valeur de `locale`  dans le `worker()` ne change pas. 

c) 
```

---

## Corrigé (à lire après avoir tenté)

<details>
<summary>Exercice 1</summary>

1. **Faux** — Les adresses virtuelles sont identiques mais mappées à des pages physiques différentes. Les valeurs sont indépendantes.
2. **Faux** — Une variable **locale** est sur la pile du thread. Chaque thread a sa propre pile. T2 ne peut pas y accéder (sauf si T1 lui passe un pointeur explicitement).
3. **Vrai** — Un crash (segfault) envoie un signal au processus entier. Par défaut, le processus est tué avec tous ses threads.
4. **Vrai** — Créer un thread alloue une pile et initialise un contexte. Créer un processus (`fork`) duplique tout l'espace d'adressage.
5. **Vrai** — Dans N:1, le noyau ne voit qu'un seul thread. Si ce thread est bloqué, tous les threads utilisateur sont gelés.

</details>

<details>
<summary>Exercice 2</summary>

a)
| Variable | Segment | Partagée |
|----------|---------|----------|
| A | .data | Oui |
| B | Stack (propre à chaque thread) | Non |
| C (pointeur) | Stack | Non |
| *heap (valeur) | Heap | Oui (si le pointeur est partagé) |
| D | Stack du main thread | Non |

b) Oui. Chaque thread a sa propre pile, donc sa propre copie de `locale`. Deux valeurs distinctes coexistent en mémoire en même temps.

c) T2 lit `100`. La variable globale est partagée. Sans synchronisation, l'ordre des accès n'est pas garanti, mais la valeur sera visible.

</details>
