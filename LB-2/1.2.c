#include <stdio.h>

int main() {
    int a = 5;
    int b = 10;

    int* pa = &a;
    int* pb = &b;

    int result = *pa + *pb;

    printf("%d\n", result);

    return 0;
}