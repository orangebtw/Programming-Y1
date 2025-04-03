#include <stdio.h>

int main() {
    int A[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    printf("Matritsa А:\n");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < 3; ++i) {
        for (int j = i; j < 3; ++j) {
            int temp = A[j][i];
            A[j][i] = A[i][j];
            A[i][j] = temp;
        }
    }
    printf("Transponirovannaya matritsa А:\n");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    
    return 0;
}
