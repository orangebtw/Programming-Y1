#include <stdio.h>

int main() {
    int a = 50;
    int b = 100;

    int* pa = &a;
    int* pb = &b;

    int max = 0;
    int* pmax = &max;
    
    if (*pa > *pb) {
        *pmax = *pa;
    } else {
        *pmax = *pb;
    }

    printf("%d\n", *pmax);

    return 0;
}