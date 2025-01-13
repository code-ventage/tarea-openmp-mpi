#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initialize_matrix(int *matrix, int rows, int cols) {
  srand(time(NULL));
  for (int i = 0; i < rows * cols; i++) {
    matrix[i] = rand() % 100; // Números aleatorios entre 0 y 99
  }
}

// Función para imprimir la matriz
void print_matrix(int *matrix, int rows, int cols) {
  printf("\nMatriz inicial\n");
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printf("%3d ", matrix[i * cols + j]);
    }
    printf("\n");
  }
}

void write_time_to_file(double time_spent) {
  FILE *time_file = fopen("time_seq.txt", "w");
  if (time_file == NULL) {
    perror("Error opening file");
    return;
  }
  fprintf(time_file, "%lf\n", time_spent);
  fclose(time_file);
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  clock_t start = clock();
  // code start
  double time_spent = 0.0;
  int rows = atoi(argv[1]), cols = atoi(argv[2]);
  int *matrix = (int *)malloc(rows * cols * sizeof(int));
  int *sum = (int *)malloc(rows * sizeof(int));
  initialize_matrix(matrix, rows, cols);
  print_matrix(matrix, rows, cols);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      sum[i] += matrix[i * cols + j];
  // code ends here
  clock_t end = clock();
  time_spent += (double)(end - start) / CLOCKS_PER_SEC;
  write_time_to_file(time_spent);
  return 0;
}
