#include <stdio.h>
#include <stdlib.h>

int main() {
    double* p = malloc(sizeof(double));
    double** p1 = malloc(sizeof(double*));
    double*** pointer = malloc(sizeof(double**));

    *p1 = p;
    *pointer = p1;
    ***pointer = 2.0;

    printf("%lf\n", *p);

    free(p);
    free(p1);
    free(pointer);

    return 0;
}
