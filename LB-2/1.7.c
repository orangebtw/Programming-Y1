#include <stdio.h>
#include <stdlib.h>

int main() {
    size_t rows, cols;

    printf("Vvedite kolichestvo strok: ");
    scanf("%zu", &rows);

    printf("Vvedite kolichestvo stolbtsov: ");
    scanf("%zu", &cols);

    double **array = malloc(rows * sizeof(double*));
    for (size_t row = 0; row < rows; ++row) {
        array[row] = (double*) malloc(cols * sizeof(double));
    }

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            printf("Vvedite element dlya %zu stroki, %zu stolbtsa: ", row + 1, col + 1);
            scanf("%lf", &array[row][col]);
        }
    }

    printf("Matritsa:\n");
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            printf("%lf ", array[row][col]);
        }
        printf("\n");
    }

    for (size_t row = 0; row < rows; ++row) {
        free(array[row]);
    }

    free(array);

    return 0;
}