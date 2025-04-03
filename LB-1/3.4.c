#include <stdio.h>

int main() {
    int A[4][4];

    for (int i = 0; i < 4; ++i) {
        printf("Vvedite %d stroku matritsi (4 elementa): ", i + 1);
        scanf("%d %d %d %d", &A[i][0], &A[i][1], &A[i][2], &A[i][3]);
    }

    printf("Ishodnaya matritsa:\n");
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < 4; ++i) {
        int sum = 0;
        for (int j = 0; j < 4; ++j) {
            sum += A[i][j];
        }

        A[i][0] = sum / 4;
    }

    printf("Preobrazovanaya matritsa:\n");
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    
    return 0;
}
