#include <stdio.h>
#include <stdlib.h>

int main() {
    size_t size;

    printf("Vvedite kolichestvo elementov v massive: ");
    scanf("%zu", &size);

    double* array = (double*) malloc(size * sizeof(double));

    for (size_t i = 0; i < size; ++i) {
        printf("Vvedite %zu element massiva: ", i + 1);
        scanf("%lf", array + i * sizeof(double));
    }

    printf("Poluchivshiysya massiv:\n");
    for (size_t i = 0; i < size; ++i) {
        printf("%lf\n", *(array + i * sizeof(double)));
    }

    free(array);

    return 0;
}