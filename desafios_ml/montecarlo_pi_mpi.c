#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    long long int lanzamientos_totales = 1000000;  // 1 millón
    long long int lanzamientos_por_proceso, aciertos_locales = 0, aciertos_globales = 0;
    double x, y, pi_estimado;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Proceso 0: distribuye el total de lanzamientos
    if (rank == 0) {
        printf("=== SIMULADOR PARALELO DE π (MONTE CARLO) ===\n");
        printf("Lanzamientos totales: %lld | Procesos: %d\n", lanzamientos_totales, size);
        fflush(stdout);
    }
    MPI_Bcast(&lanzamientos_totales, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Cada proceso calcula su cuota de lanzamientos
    lanzamientos_por_proceso = lanzamientos_totales / size;
    srand(time(NULL) + rank);  // Semilla única por proceso

    for (long long int i = 0; i < lanzamientos_por_proceso; i++) {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;
        if (x*x + y*y <= 1.0) aciertos_locales++;
    }

    printf("Proceso %d: aciertos = %lld\n", rank, aciertos_locales);
    fflush(stdout);

    // Reducción: sumar todos los aciertos en el proceso 0
    MPI_Reduce(&aciertos_locales, &aciertos_globales, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        pi_estimado = 4.0 * (double)aciertos_globales / (double)lanzamientos_totales;
        printf("\n🔵 Aciertos totales: %lld\n", aciertos_globales);
        printf("🔵 π estimado: %.6f\n", pi_estimado);
        printf("🔵 Error absoluto: %.6f\n", pi_estimado - 3.141592653589793);
    }

    MPI_Finalize();
    return 0;
}
