#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    long long int total_lanzamientos = 1000000;
    long long int lanzamientos_local, aciertos_local = 0, aciertos_global = 0;
    double x, y, pi_estimado;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Broadcast del total de lanzamientos
    MPI_Bcast(&total_lanzamientos, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    lanzamientos_local = total_lanzamientos / size;
    std::srand(std::time(nullptr) + rank);  // semilla única

    for (long long int i = 0; i < lanzamientos_local; ++i) {
        x = static_cast<double>(std::rand()) / RAND_MAX;
        y = static_cast<double>(std::rand()) / RAND_MAX;
        if (x*x + y*y <= 1.0) ++aciertos_local;
    }

    std::cout << "Proceso " << rank << " aciertos: " << aciertos_local << std::endl;

    MPI_Reduce(&aciertos_local, &aciertos_global, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        pi_estimado = 4.0 * static_cast<double>(aciertos_global) / total_lanzamientos;
        std::cout << "\n=== ESTIMACIÓN DE π ===" << std::endl;
        std::cout << "Aciertos totales: " << aciertos_global << std::endl;
        std::cout << "π ≈ " << pi_estimado << std::endl;
        std::cout << "Error: " << pi_estimado - 3.141592653589793 << std::endl;
    }

    MPI_Finalize();
    return 0;
}
