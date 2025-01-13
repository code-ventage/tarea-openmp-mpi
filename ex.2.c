#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Función para calcular el producto escalar de dos vectores
double dot_product(double *a, double *b, int n) {
  double sum = 0.0;

#pragma omp parallel for reduction(+ : sum)
  for (int i = 0; i < n; i++) {
    sum += a[i] * b[i];
  }

  return sum;
}

// Función para inicializar un vector con valores aleatorios
void initialize_random_vector(double *vec, int n) {
  for (int i = 0; i < n; i++) {
    vec[i] = rand() % 100; // Valores entre 0 y 99
  }
}

double read_seq_time_from_file() {
  FILE *time_file = fopen("time_seq_2.txt", "r");
  double seq_time = 0.0;
  fscanf(time_file, "%lf", &seq_time);
  return seq_time;
}

int main(int argc, char *argv[]) {
  int rank, size;
  int n; // Tamaño del vector
  double *vec_a = NULL, *vec_b = NULL;
  double local_sum = 0.0, global_sum = 0.0;

  double start_time, end_time;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Proceso maestro lee el tamaño del vector desde la consola
  if (rank == 0) {
    printf("Ingrese el tamaño del vector: ");
    scanf("%d", &n);
  }
  // Transmitir el tamaño del vector a todos los procesos MPI_Bcast(&n, 1,
  // MPI_INT, 0, MPI_COMM_WORLD);

  // División del trabajo
  int local_n = n / size; // Tamaño del subvector para cada proceso
  if (rank == size - 1)
    local_n += n % size; // Último proceso ajusta el tamaño

  double *local_a = (double *)malloc(local_n * sizeof(double));
  double *local_b = (double *)malloc(local_n * sizeof(double));

  // Inicialización de los vectores en el proceso maestro
  if (rank == 0) {
    srand(time(NULL)); // Semilla para valores aleatorios
    vec_a = (double *)malloc(n * sizeof(double));
    vec_b = (double *)malloc(n * sizeof(double));
    initialize_random_vector(vec_a, n);
    initialize_random_vector(vec_b, n);

    // Imprimir los vectores para verificar
    printf("Vector A: ");
    for (int i = 0; i < n; i++)
      printf("%.2f ", vec_a[i]);
    printf("\n");

    printf("Vector B: ");
    for (int i = 0; i < n; i++)
      printf("%.2f ", vec_b[i]);
    printf("\n");
  }

  // Distribuir los datos a los procesos
  MPI_Scatter(vec_a, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0,
              MPI_COMM_WORLD);
  MPI_Scatter(vec_b, local_n, MPI_DOUBLE, local_b, local_n, MPI_DOUBLE, 0,
              MPI_COMM_WORLD);

  // Calcular producto escalar local
  start_time = MPI_Wtime();
  local_sum = dot_product(local_a, local_b, local_n);
  end_time = MPI_Wtime();

  // Reducir los resultados locales en el proceso maestro
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);

  // Mostrar el resultado en el proceso maestro
  if (rank == 0) {
    printf("Producto escalar: %f\n", global_sum);
    free(vec_a);
    free(vec_b);
  }

  if (rank == 0) {
    double time_spent = end_time - start_time;
    double seq_time = read_seq_time_from_file();
    printf("Speedup: %lf\n", seq_time / time_spent);
    double efficiency = (seq_time / time_spent) / size;
    printf("Eficiencia: %lf\n", efficiency);
  }

  free(local_a);
  free(local_b);
  MPI_Finalize();

  return 0;
}
