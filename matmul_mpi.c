#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#define N 4  // Tamaño manejable para análisis

int main(int argc, char *argv[]) {
    int rank, size;
    int A[N][N], B[N][N], C[N][N];
    int fila[N], resultado_parcial[N];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Configurar semilla aleatoria diferente en cada proceso
    srand(time(NULL) + rank);
    
    // Generar datos aleatorios en todos los procesos
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }

    // Sincronizar procesos antes de imprimir
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Cada proceso imprime sus datos locales
    for (int r = 0; r < size; r++) {
        if (rank == r) {
            printf("\nProceso %d - Matriz local A antes de distribuir:\n", rank);
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    printf("%d ", A[i][j]);
                }
                printf("\n");
            }
            
            printf("\nProceso %d - Matriz local B antes de distribuir:\n", rank);
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    printf("%d ", B[i][j]);
                }
                printf("\n");
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Distribuir trabajo
    MPI_Scatter(A, N*N/size, MPI_INT, fila, N*N/size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, N*N, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Mostrar datos recibidos en cada proceso
    for (int r = 0; r < size; r++) {
        if (rank == r) {
            printf("\nProceso %d - Fila recibida para calcular:\n", rank);
            for (int j = 0; j < N; j++) {
                printf("%d ", fila[j]);
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Computo paralelo - cada proceso calcula su parte
    for (int j = 0; j < N; j++) {
        resultado_parcial[j] = 0;
        for (int k = 0; k < N; k++) {
            resultado_parcial[j] += fila[k] * B[k][j];
        }
    }
    
    // Mostrar resultado parcial en cada proceso
    for (int r = 0; r < size; r++) {
        if (rank == r) {
            printf("\nProceso %d - Resultado parcial calculado:\n", rank);
            for (int j = 0; j < N; j++) {
                printf("%d ", resultado_parcial[j]);
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Recolección
    MPI_Gather(resultado_parcial, N, MPI_INT, C, N, MPI_INT, 0, MPI_COMM_WORLD);

    // Resultado final - solo el proceso 0 imprime
    if (rank == 0) {
        // Imprimir matriz A final (después de distribución)
        printf("\n\nMATRIZ A COMPLETA:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        
        // Imprimir matriz B final
        printf("\nMATRIZ B COMPLETA:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
        
        // Imprimir matriz resultado
        printf("\nRESULTADO C = A x B:\n");
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
