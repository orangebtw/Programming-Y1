#include <stdio.h>
#include <math.h>

int main() {
    float a, b, c, x;
    
    printf("Vvedite a: ");
    scanf("%f", &a);

    printf("Vvedite b: ");
    scanf("%f", &b);

    printf("Vvedite c: ");
    scanf("%f", &c);

    printf("Vvedite x: ");
    scanf("%f", &x);

    float q = x*x + b*b;

    float h = -(x - a) / powf(x*x + a*a, 1.0f / 3.0f) - (4.0f * powf(q*q*q, 1.0f / 4.0f)) / (2.0f + a + b + powf((x-c)*(x-c), 1.0f / 3.0f)); 
    printf("%f", h);
    
    return 0;
}
