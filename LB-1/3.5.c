#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10

int main() {
    srand(time(NULL));
    
    int M[SIZE];

    for (int i = 0; i < SIZE; ++i) {
        M[i] = rand() % 100;
    }

    printf("Ishodniy massiv:\n");
    for (int i = 0; i < SIZE; ++i) {
        printf("%d ", M[i]);
    }
    printf("\n");

    for (int i = 1; i < SIZE; ++i) {
        for (int j = i; j > 0; --j) {
            if (M[j - 1] > M[j]) {
                int temp = M[j - 1];
                M[j - 1] = M[j];
                M[j] = temp;
            } else {
                break;
            }
        }
    }

    printf("Otsortirovanniy massiv:\n");
    for (int i = 0; i < SIZE; ++i) {
        printf("%d ", M[i]);
    }
    printf("\n");
    
    return 0;
}
