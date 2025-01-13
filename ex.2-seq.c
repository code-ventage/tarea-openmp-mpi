#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generate_random_vector(int *vec, int n) {
  srand(time(NULL));
  for (int i = 0; i < n; ++i)
    vec[i] = rand() % 100;
}

void print_vector(int *vec, int n) {
  printf("Vector: ");
  for (int i = 0; i < n; ++i)
    printf("%d ", vec[i]);
  printf("\n");
}

void write_time_to_file(double time_spent) {
  FILE *time_file = fopen("time_seq_2.txt", "w");
  if (time_file == NULL) {
    perror("Error opening file");
    return;
  }
  fprintf(time_file, "%lf\n", time_spent);
  fclose(time_file);
}

int main(int argc, char *argv[]) {
  clock_t start = clock();
  double time_spent = 0.0;
  // logic starts here
  int n;
  printf("Ingrese el tamaño del vector: ");
  scanf("%d", &n);

  int *vector_a = (int *)malloc(n * sizeof(int));
  int *vector_b = (int *)malloc(n * sizeof(int));
  generate_random_vector(vector_a, n);
  generate_random_vector(vector_b, n);
  
  int scalar_product = 0;
  for (int i = 0; i < n; ++i) {
    scalar_product += vector_a[i] * vector_b[i];
  }

  // logic ends here
  clock_t end = clock();
  time_spent += (double)(end - start) / CLOCKS_PER_SEC;
  write_time_to_file(time_spent);

  return 0;
}
