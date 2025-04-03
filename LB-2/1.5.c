#include <stdio.h>
#include <stdlib.h>

int main() {
    size_t size;

    printf("Vvedite kolichestvo elementov v massive: ");
    scanf("%zu", &size);

    double* array = (double*) malloc(size * sizeof(double));
    double* parray = array;

    for (size_t i = 0; i < size; ++i) {
        printf("Vvedite %zu element massiva: ", i + 1);
        scanf("%lf", parray);
        ++parray;
    }

    printf("Poluchivshiysya massiv v obratnom poryadke:\n");
    for (size_t i = 0; i < size; ++i) {
        --parray;
        printf("%lf\n", *parray);
    }

    free(array);

    return 0;
}