#include <stdio.h>

int main() {
    float X[3];

    printf("Vvedite vector iz tryoh elementov: ");
    scanf("%f %f %f", &X[0], &X[1], &X[2]);

    float Y[3];

    for (int i = 0; i < 3; ++i) {
        Y[i] = X[i] * X[i];
    }
    
    printf("%f %f %f\n", Y[0], Y[1], Y[2]);

    return 0;
}
