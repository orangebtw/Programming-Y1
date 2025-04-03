#include <stdio.h>

int main() {
    unsigned long long m;

    printf("Vvedite m: ");
    scanf("%llu", &m);

    unsigned long long first = 1;
    unsigned long long second = 1;
    unsigned long long third = 1;
    unsigned long long fourth = 2;
    
    for (unsigned long long n = 0; fourth <= m; ++n) {
        if (n <= 2) {
            printf("%llu\n", 1);
        } else {
            printf("%llu\n", fourth);
            fourth = first + second;
            first = second;
            second = third;
            third = fourth;
        }
    }
    
    return 0;
}
