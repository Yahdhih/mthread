# Choix de l'Application Démo

## Option A — Serveur HTTP Minimal

**Ce que ça fait** : écoute sur le port 8080, répond "Hello World" à curl.
Chaque connexion est gérée par un thread mthread.

```bash
# Test :
curl http://localhost:8080/
# Attendu : Hello World
```

**Niveau** : difficile (sockets, bind/accept/recv/send)
**Intérêt** : montre les threads N:1 sur un vrai cas réseau

## Option B — Tri Fusion Parallèle

**Ce que ça fait** : trie un tableau de N entiers en le divisant en K segments,
chaque segment trié par un thread, puis fusion finale.

```c
// Structure :
// main : divise, crée K threads, join, fusionne
// thread : sort(arr + offset, len)
```

**Niveau** : moyen (algorithme classique)
**Intérêt** : benchmark naturel (speedup vs séquentiel)

## Option C — Simulateur M/M/1

**Ce que ça fait** : simule une file d'attente avec λ arrivées/s et μ services/s.
Compare avec la formule théorique W = 1/(μ-λ).

**Niveau** : moyen-difficile (aléatoire, timing)
**Intérêt** : valide la bibliothèque sur un problème scientifique

---

## Mon choix :
[ ] Option A — HTTP
[ ] Option B — Tri fusion
[ ] Option C — M/M/1

## Pourquoi :
```
