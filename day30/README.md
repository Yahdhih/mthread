# Jour 30 — Projet : Setup et structure
**Durée : 1h** | Phase 4 — Projet Final

---

## Programme

| Activité | Fichier | Durée |
|----------|---------|-------|
| Lire l'API finale | `libmthread.h` | 15 min |
| Créer la structure du projet | `Makefile` | 20 min |
| Copier tes implémentations existantes | `src/` | 25 min |

## Structure à créer
```
projet/         ← créer ce répertoire dans day30/
├── src/
│   ├── mthread.c
│   ├── mthread_mutex.c
│   ├── mthread_sem.c
│   ├── mthread_cond.c
│   └── mthread_preempt.c
├── include/
│   └── mthread.h
├── tests/
│   └── (vide pour l'instant)
├── libmthread.h  ← API publique complète
└── Makefile
```

## À faire aujourd'hui
1. Créer les répertoires : `mkdir -p projet/src projet/include projet/tests`
2. Copier tes meilleurs fichiers depuis les TP dans `projet/src/`
3. Vérifier que `make lib` compile : `ar rcs libmthread.a src/*.o`

---
➡️ Demain : `day31/` — Consolider mthread.c (version définitive)
