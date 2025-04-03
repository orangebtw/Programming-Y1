#include <stdio.h>
#include <math.h>

static double f(double x) {
    return exp(x + 2.0);
}

int main() {
    double a, b;
    int n;

    printf("Vvedite a: ");
    scanf("%lf", &a);

    printf("Vvedite b: ");
    scanf("%lf", &b);

    printf("Vvedite n: ");
    scanf("%d", &n);
    
    double h = (b - a) / n;
    double result = (f(a) + f(b)) / 2.0;
    for (double x = a + h; x <= b - h; x += h) {
        result += f(x);
    }
    
    result *= h;
    
    printf("%lf\n", result);
    
    return 0;
}
