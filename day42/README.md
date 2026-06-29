# Jour 42 — Projet : Valgrind propre
**Durée : 1h** | Phase 4 — Projet Final

## Programme
Faire tourner valgrind sur chaque test et corriger les leaks.

```bash
valgrind --leak-check=full --show-leak-kinds=all ./test_threads
valgrind --leak-check=full --show-leak-kinds=all ./test_mutex
valgrind --leak-check=full --show-leak-kinds=all ./test_sem
valgrind --leak-check=full --show-leak-kinds=all ./test_cond
valgrind --leak-check=full --show-leak-kinds=all ./test_stress
valgrind --leak-check=full --show-leak-kinds=all ./test_philosophes
```

## Leaks courants à corriger
- Pile du scheduler non libérée → garder une référence et free à la fin
- Thread ZOMBIE non free → vérifier que join libère bien stack + struct

---
➡️ Demain : day43/ — Application démo finalisée
