#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#define N 4

int main(int argc, char *argv[]) {
    int rank, size;
    int A[N][N], B[N][N], C[N][N];
    int fila[N], resultado_parcial[N];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        srand(time(NULL));
        printf("Matriz A generada por proceso 0:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = rand() % 10;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        
        printf("\nMatriz B generada por proceso 0:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j] = rand() % 10;
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Scatter(A, N*N/size, MPI_INT, fila, N*N/size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, N*N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    
    for (int r = 0; r < size; r++) {
        if (rank == r) {
            printf("\nProceso %d - Fila recibida de A: ", rank);
            for (int j = 0; j < N; j++) {
                printf("%d ", fila[j]);
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    for (int j = 0; j < N; j++) {
        resultado_parcial[j] = 0;
        for (int k = 0; k < N; k++) {
            resultado_parcial[j] += fila[k] * B[k][j];
        }
    }
    
    for (int r = 0; r < size; r++) {
        if (rank == r) {
            printf("\nProceso %d - Resultado parcial (fila de C): ", rank);
            for (int j = 0; j < N; j++) {
                printf("%d ", resultado_parcial[j]);
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Gather(resultado_parcial, N, MPI_INT, C, N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\n\nMatriz resultado C:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
