#include <stdio.h>

typedef struct {
    int (*ptr)(int, int);
} Function;

int add(int a, int b) {
    return a + b;
}

int main() {
    Function f = {
        .ptr = add
    };

    int result = f.ptr(10, 10);

    printf("%d\n", result);

    return 0;
}