#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define N 4  // Matrices 4x4 (ajustable para pruebas)

int main(int argc, char **argv) {
    int rank, size;
    int A[N][N], B[N][N], C[N][N], fila[N], resultado[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Solo el proceso 0 inicializa las matrices
    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = rand() % 10;
                B[i][j] = rand() % 10;
            }
        }
    }

    // Distribuir filas de A a todos los procesos
    MPI_Scatter(A, N*N/size, MPI_INT, fila, N*N/size, MPI_INT, 0, MPI_COMM_WORLD);
    // Broadcast de la matriz B completa a todos
    MPI_Bcast(B, N*N, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso calcula una fila de C
    for (int j = 0; j < N; j++) {
        resultado[j] = 0;
        for (int k = 0; k < N; k++) {
            resultado[j] += fila[k] * B[k][j];  // Multiplicación fila x columna
        }
    }

    // Recolectar resultados en el proceso 0
    MPI_Gather(resultado, N, MPI_INT, C, N, MPI_INT, 0, MPI_COMM_WORLD);

    // Solo el proceso 0 imprime el resultado
    if (rank == 0) {
        printf("Matriz C (Resultado):\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) printf("%d\t", C[i][j]);
            printf("\n");
        }
    }
    MPI_Finalize();
}
