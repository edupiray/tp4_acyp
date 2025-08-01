#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

const int N = 4;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<int> A, B, C;
    if (rank == 0) {
        A.resize(N*N);
        B.resize(N*N);
        C.resize(N*N);
        
        std::srand(std::time(nullptr));
        std::cout << "Matriz A generada por proceso 0:\n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i*N+j] = std::rand() % 10;
                std::cout << A[i*N+j] << " ";
            }
            std::cout << "\n";
        }
        
        std::cout << "\nMatriz B generada por proceso 0:\n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i*N+j] = std::rand() % 10;
                std::cout << B[i*N+j] << " ";
            }
            std::cout << "\n";
        }
    }
    else {
        B.resize(N*N);
    }

    std::vector<int> fila(N);
    std::vector<int> resultado_parcial(N);

    MPI_Scatter(
        rank == 0 ? A.data() : nullptr,
        N*N/size,
        MPI_INT,
        fila.data(),
        N*N/size,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    MPI_Bcast(
        B.data(),
        N*N,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    MPI_Barrier(MPI_COMM_WORLD);
    
    for (int r = 0; r < size; r++) {
        if (rank == r) {
            std::cout << "\nProceso " << rank << " - Fila recibida de A:";
            for (int j = 0; j < N; j++) {
                std::cout << " " << fila[j];
            }
            std::cout << std::endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    for (int j = 0; j < N; j++) {
        resultado_parcial[j] = 0;
        for (int k = 0; k < N; k++) {
            resultado_parcial[j] += fila[k] * B[k*N+j];
        }
    }
    
    for (int r = 0; r < size; r++) {
        if (rank == r) {
            std::cout << "\nProceso " << rank << " - Resultado parcial (fila de C):";
            for (int j = 0; j < N; j++) {
                std::cout << " " << resultado_parcial[j];
            }
            std::cout << std::endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Gather(
        resultado_parcial.data(), 
        N, 
        MPI_INT,
        rank == 0 ? C.data() : nullptr,
        N, 
        MPI_INT,
        0, 
        MPI_COMM_WORLD
    );

    if (rank == 0) {
        std::cout << "\n\nMatriz resultado C:\n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                std::cout << C[i*N+j] << " ";
            }
            std::cout << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}
