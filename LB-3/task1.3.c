#include <stdio.h>
#include <complex.h>

int main() {
    double complex z = 4 + 5*I;
    double complex result = cexp(z);

    printf("z = %f + i%f\n", creal(z), cimag(z));
    printf("exp(z) = %f + i%f\n", creal(result), cimag(result));

    return 0;
}