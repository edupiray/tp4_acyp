# Desafíos de Machine Learning con MPI

Esta carpeta contiene dos algoritmos implementados en paralelo con MPI:

- **Montecarlo para estimar π** (simulación aleatoria)
- **K-means clustering** (agrupamiento no supervisado)

## Instrucciones de ejecución

### 1. Montecarlo π

Compilar (C o C++):
```bash
# Versión C
mpicc montecarlo_pi_mpi.c -o montecarlo_c
# Versión C++
mpic++ montecarlo_pi_mpi.cpp -o montecarlo_cpp

```
Ejecutar por ejemplo con 4 procesos:
```bash
mpirun -np 4 ./montecarlo_c   # o ./montecarlo_cpp
```
Nota: El número de procesos puede ser cualquiera (no requiere divisibilidad exacta).
