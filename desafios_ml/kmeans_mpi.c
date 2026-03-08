#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

#define N_PUNTOS 1000    // Filas
#define DIM 2            // Columnas (x,y)
#define K 3              // Número de clusters
#define MAX_ITER 20

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // ---------- PROCESO 0: GENERAR DATOS ALEATORIOS ----------
    double *datos = NULL;
    double centros_actuales[K][DIM], centros_nuevos[K][DIM];
    int *cuentas_locales = NULL, *cuentas_globales = NULL;
    double *sumas_locales = NULL, *sumas_globales = NULL;

    if (rank == 0) {
        datos = (double*)malloc(N_PUNTOS * DIM * sizeof(double));
        srand(time(NULL));
        printf("=== K-MEANS PARALELO CON MPI ===\n");
        for (int i = 0; i < N_PUNTOS * DIM; i++)
            datos[i] = (double)rand() / RAND_MAX * 10.0;

        // Inicializar K centros aleatorios
        for (int k = 0; k < K; k++) {
            int idx = rand() % N_PUNTOS;
            centros_actuales[k][0] = datos[idx * DIM];
            centros_actuales[k][1] = datos[idx * DIM + 1];
        }
        printf("Centros iniciales: \n");
        for (int k = 0; k < K; k++)
            printf("C%d: (%.2f, %.2f)\n", k, centros_actuales[k][0], centros_actuales[k][1]);
    }

    // ---------- BROADCAST DE CENTROS INICIALES A TODOS ----------
    MPI_Bcast(centros_actuales, K * DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // ---------- DISTRIBUIR PUNTOS (SCATTER) ----------
    int puntos_por_proc = N_PUNTOS / size;
    double *datos_locales = (double*)malloc(puntos_por_proc * DIM * sizeof(double));
    MPI_Scatter(datos, puntos_por_proc * DIM, MPI_DOUBLE,
                datos_locales, puntos_por_proc * DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Buffers para sumas parciales
    sumas_locales = (double*)calloc(K * DIM, sizeof(double));
    cuentas_locales = (int*)calloc(K, sizeof(int));
    sumas_globales = (double*)calloc(K * DIM, sizeof(double));
    cuentas_globales = (int*)calloc(K, sizeof(int));

    // ---------- ITERACIONES DEL ALGORITMO ----------
    for (int iter = 0; iter < MAX_ITER; iter++) {
        // 1. Reiniciar acumuladores locales
        for (int k = 0; k < K; k++) {
            cuentas_locales[k] = 0;
            sumas_locales[k * DIM] = 0;
            sumas_locales[k * DIM + 1] = 0;
        }

        // 2. Asignar cada punto local al centro más cercano
        for (int i = 0; i < puntos_por_proc; i++) {
            double x = datos_locales[i * DIM];
            double y = datos_locales[i * DIM + 1];
            int cluster_asignado = 0;
            double min_dist = INFINITY;

            for (int k = 0; k < K; k++) {
                double dx = x - centros_actuales[k][0];
                double dy = y - centros_actuales[k][1];
                double dist = dx*dx + dy*dy;
                if (dist < min_dist) {
                    min_dist = dist;
                    cluster_asignado = k;
                }
            }
            cuentas_locales[cluster_asignado]++;
            sumas_locales[cluster_asignado * DIM] += x;
            sumas_locales[cluster_asignado * DIM + 1] += y;
        }

        // 3. Reducción global: sumar cuentas y coordenadas de todos los procesos
        MPI_Allreduce(cuentas_locales, cuentas_globales, K, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(sumas_locales, sumas_globales, K * DIM, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        // 4. Proceso 0: actualizar centros
        if (rank == 0) {
            for (int k = 0; k < K; k++) {
                if (cuentas_globales[k] > 0) {
                    centros_nuevos[k][0] = sumas_globales[k * DIM] / cuentas_globales[k];
                    centros_nuevos[k][1] = sumas_globales[k * DIM + 1] / cuentas_globales[k];
                }
            }
            // Verificar convergencia (aquí simplificado: copiar siempre)
            for (int k = 0; k < K; k++) {
                centros_actuales[k][0] = centros_nuevos[k][0];
                centros_actuales[k][1] = centros_nuevos[k][1];
            }
        }

        // 5. Broadcast de los nuevos centros
        MPI_Bcast(centros_actuales, K * DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // ---------- RESULTADO FINAL ----------
    if (rank == 0) {
        printf("\n✅ Centros finales después de %d iteraciones:\n", MAX_ITER);
        for (int k = 0; k < K; k++)
            printf("Cluster %d: (%.2f, %.2f) - %d puntos\n",
                   k, centros_actuales[k][0], centros_actuales[k][1], cuentas_globales[k]);
    }

    free(datos_locales); free(sumas_locales); free(cuentas_locales);
    free(sumas_globales); free(cuentas_globales);
    if (rank == 0) free(datos);
    MPI_Finalize();
    return 0;
}
