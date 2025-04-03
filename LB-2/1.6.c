#include <stdio.h>
#include <stdlib.h>

int main() {
    int a = 1234567890;

    char* pa = (char*) &a;

    for (size_t i = 0; i < sizeof(a); ++i) {
        printf("%d ", pa[i]);
    }
    printf("\n");

    return 0;
}