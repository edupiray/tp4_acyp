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

### 2. K-means clustering
Importante: El número de puntos (N_PUNTOS = 1000) debe ser divisible por la cantidad de procesos. Usar valores como -np 2, 4, 5, 8, 10.

Compilar (C o C++):
```bash
# Versión C
mpicc kmeans_mpi.c -o kmeans_c -lm
# Versión C++
mpic++ kmeans_mpi.cpp -o kmeans_cpp -lm
```
Ejecutar por ejemplo con 4 procesos:
```bash
mpirun -np 4 ./kmeans_c  # o ./kmeans_cpp
```

