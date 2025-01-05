
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Función para inicializar la matriz con valores aleatorios
void initialize_matrix(int *matrix, int rows, int cols) {
  srand(time(NULL));
  for (int i = 0; i < rows * cols; i++) {
    matrix[i] = rand() % 100; // Números aleatorios entre 0 y 99
  }
}

// Función para imprimir la matriz
void print_matrix(int *matrix, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printf("%3d ", matrix[i * cols + j]);
    }
    printf("\n");
  }
}

// Función para imprimir un vector
void print_vector(int *vector, int size) {
  for (int i = 0; i < size; i++) {
    printf("%d ", vector[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  int rank, size;
  int rows, cols;

  // Inicializar MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Verificar los argumentos de entrada
  if (argc != 3) {
    if (rank == 0) {
      printf("Uso: %s <número de filas> <número de columnas>\n", argv[0]);
    }
    MPI_Finalize();
    return -1;
  }

  rows = atoi(argv[1]);
  cols = atoi(argv[2]);

  int *matrix = NULL;
  int *local_matrix = NULL;
  int *local_sums = NULL;
  int *global_sums = NULL;

  // Proceso 0 inicializa la matriz
  if (rank == 0) {
    matrix = (int *)malloc(rows * cols * sizeof(int));
    global_sums = (int *)malloc(rows * sizeof(int));
    initialize_matrix(matrix, rows, cols);
    printf("Matriz inicial:\n");
    print_matrix(matrix, rows, cols);
  }

  // Calcular cuántas filas le tocan a cada proceso
  int *send_counts = (int *)malloc(
      size * sizeof(int)); // Número de elementos a enviar a cada proceso
  int *displs =
      (int *)malloc(size * sizeof(int)); // Desplazamientos para cada proceso
  int rows_per_process = rows / size;
  int remaining_rows = rows % size;

  for (int i = 0; i < size; i++) {
    send_counts[i] = (rows_per_process + (i < remaining_rows ? 1 : 0)) * cols;
    displs[i] = (i == 0) ? 0 : displs[i - 1] + send_counts[i - 1];
  }

  // Cada proceso aloja espacio para sus filas locales
  int local_rows = send_counts[rank] / cols;
  local_matrix = (int *)malloc(send_counts[rank] * sizeof(int));
  local_sums = (int *)malloc(local_rows * sizeof(int));

  // Distribuir partes de la matriz a cada proceso
  MPI_Scatterv(matrix, send_counts, displs, MPI_INT, local_matrix,
               send_counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

// Calcular la suma de las filas localmente usando OpenMP
#pragma omp parallel for
  for (int i = 0; i < local_rows; i++) {
    local_sums[i] = 0;
    for (int j = 0; j < cols; j++) {
      local_sums[i] += local_matrix[i * cols + j];
    }
  }

  // Reunir las sumas locales en el proceso 0
  int *recv_counts =
      (int *)malloc(size * sizeof(int)); // Filas a recibir de cada proceso
  int *recv_displs =
      (int *)malloc(size * sizeof(int)); // Desplazamientos para recopilar

  for (int i = 0; i < size; i++) {
    recv_counts[i] = send_counts[i] / cols; // Convertir elementos en filas
    recv_displs[i] = (i == 0) ? 0 : recv_displs[i - 1] + recv_counts[i - 1];
  }

  MPI_Gatherv(local_sums, local_rows, MPI_INT, global_sums, recv_counts,
              recv_displs, MPI_INT, 0, MPI_COMM_WORLD);

  // Proceso 0 imprime el resultado
  if (rank == 0) {
    printf("Vector de sumas por fila:\n");
    print_vector(global_sums, rows);
  }

  // Liberar memoria
  if (rank == 0) {
    free(matrix);
    free(global_sums);
  }

  free(local_matrix);
  free(local_sums);
  free(send_counts);
  free(displs);
  free(recv_counts);
  free(recv_displs);

  // Finalizar MPI
  MPI_Finalize();
  return 0;
}
