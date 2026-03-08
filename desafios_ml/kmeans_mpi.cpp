#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <mpi.h>

const int N_PUNTOS = 1000;   // debe ser múltiplo del número de procesos
const int DIM = 2;
const int K = 3;
const int MAX_ITER = 20;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Verificar divisibilidad
    if (N_PUNTOS % size != 0 && rank == 0) {
        std::cerr << "Error: N_PUNTOS (" << N_PUNTOS 
                  << ") debe ser múltiplo del número de procesos (" << size << ")" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    std::vector<double> datos;
    std::vector<double> centros_actuales(K * DIM);
    std::vector<double> centros_nuevos(K * DIM);
    std::vector<int> cuentas_globales(K);
    std::vector<double> sumas_globales(K * DIM);

    // Proceso 0: generar datos y centros iniciales
    if (rank == 0) {
        datos.resize(N_PUNTOS * DIM);
        std::srand(std::time(nullptr));
        for (int i = 0; i < N_PUNTOS * DIM; ++i)
            datos[i] = static_cast<double>(std::rand()) / RAND_MAX * 10.0;

        // Seleccionar K centros aleatorios entre los puntos
        for (int k = 0; k < K; ++k) {
            int idx = std::rand() % N_PUNTOS;
            centros_actuales[k * DIM]     = datos[idx * DIM];
            centros_actuales[k * DIM + 1] = datos[idx * DIM + 1];
        }

        std::cout << "=== K-MEANS PARALELO (C++) ===" << std::endl;
        std::cout << "Puntos: " << N_PUNTOS << ", Procesos: " << size << std::endl;
        std::cout << "Centros iniciales:" << std::endl;
        for (int k = 0; k < K; ++k)
            std::cout << "C" << k << ": (" << centros_actuales[k*DIM] << ", " 
                      << centros_actuales[k*DIM+1] << ")" << std::endl;
    }

    // Broadcast de centros iniciales
    MPI_Bcast(centros_actuales.data(), K * DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Scatter de puntos
    int puntos_por_proc = N_PUNTOS / size;
    std::vector<double> datos_locales(puntos_por_proc * DIM);
    MPI_Scatter(datos.data(), puntos_por_proc * DIM, MPI_DOUBLE,
                datos_locales.data(), puntos_por_proc * DIM, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    // Buffers locales
    std::vector<int> cuentas_locales(K, 0);
    std::vector<double> sumas_locales(K * DIM, 0.0);

    // Iteraciones
    for (int iter = 0; iter < MAX_ITER; ++iter) {
        // Reiniciar acumuladores locales
        std::fill(cuentas_locales.begin(), cuentas_locales.end(), 0);
        std::fill(sumas_locales.begin(), sumas_locales.end(), 0.0);

        // Asignar puntos locales al centro más cercano
        for (int i = 0; i < puntos_por_proc; ++i) {
            double x = datos_locales[i * DIM];
            double y = datos_locales[i * DIM + 1];
            int mejor_cluster = 0;
            double min_dist = std::numeric_limits<double>::max();

            for (int k = 0; k < K; ++k) {
                double dx = x - centros_actuales[k * DIM];
                double dy = y - centros_actuales[k * DIM + 1];
                double dist = dx*dx + dy*dy;
                if (dist < min_dist) {
                    min_dist = dist;
                    mejor_cluster = k;
                }
            }
            cuentas_locales[mejor_cluster]++;
            sumas_locales[mejor_cluster * DIM]     += x;
            sumas_locales[mejor_cluster * DIM + 1] += y;
        }

        // Reducción global
        MPI_Allreduce(cuentas_locales.data(), cuentas_globales.data(), K, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(sumas_locales.data(), sumas_globales.data(), K * DIM, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        // Proceso 0: actualizar centros
        if (rank == 0) {
            for (int k = 0; k < K; ++k) {
                if (cuentas_globales[k] > 0) {
                    centros_nuevos[k * DIM]     = sumas_globales[k * DIM] / cuentas_globales[k];
                    centros_nuevos[k * DIM + 1] = sumas_globales[k * DIM + 1] / cuentas_globales[k];
                }
            }
            // Copiar nuevos centros
            centros_actuales = centros_nuevos;
        }

        // Broadcast de centros actualizados
        MPI_Bcast(centros_actuales.data(), K * DIM, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Resultado final
    if (rank == 0) {
        std::cout << "\n✅ Centros finales tras " << MAX_ITER << " iteraciones:" << std::endl;
        for (int k = 0; k < K; ++k) {
            std::cout << "Cluster " << k << ": (" 
                      << centros_actuales[k * DIM] << ", " 
                      << centros_actuales[k * DIM + 1] << ")  -  "
                      << cuentas_globales[k] << " puntos" << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
