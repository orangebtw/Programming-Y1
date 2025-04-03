#include <stdio.h>
#include <math.h>

int main() {
    float x, y;
    
    printf("Vvedite x: ");
    scanf("%f", &x);

    printf("Vvedite y: ");
    scanf("%f", &y);

    float up = 1.0f + sinf(x + y) * sinf(x + y);
    float bottom = 2.0f + fabsf(x - (2.0f * x*x) / (1.0f + fabsf(sinf(x+y))));
    float result = up / bottom;

    printf("%f", result);
    
    return 0;
}
