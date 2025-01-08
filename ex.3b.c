
#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.141592653589793238462643

int main(int argc, char *argv[]) {
  int rank, size, i;
  long num_steps;
  double step, local_sum = 0.0, global_sum = 0.0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Leer el número de pasos desde la consola en el proceso maestro
  if (rank == 0) {
    printf("Ingrese el número de pasos (num_steps): ");
    scanf("%ld", &num_steps);
  }

  // Compartir el valor de num_steps con todos los procesos
  MPI_Bcast(&num_steps, 1, MPI_LONG, 0, MPI_COMM_WORLD);

  // Calcular el tamaño del paso
  step = 1.0 / (double)num_steps;

  // Dividir el trabajo entre los procesos
  long local_n = num_steps / size; // Cantidad de iteraciones por proceso
  long start = rank * local_n;
  long end = (rank == size - 1) ? num_steps : start + local_n;

// Calcular la suma local con OpenMP
#pragma omp parallel for reduction(+ : local_sum)
  for (i = start; i < end; i++) {
    double x = (i + 0.5) * step;
    local_sum += 4.0 / (1.0 + x * x);
  }

  // Reducir las sumas locales para obtener la suma global
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);

  // Calcular pi y el error en el proceso maestro
  if (rank == 0) {
    double pi = step * global_sum;
    double error = fabs(pi - PI);

    printf("Pi calculado: %.15f\n", pi);
    printf("Error cometido: %.15e\n", error);
  }

  MPI_Finalize();
  return 0;
}
