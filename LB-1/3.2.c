#include <stdio.h>

#define SIZE 10

int main() {
    int X[SIZE];

    for (int i = 0; i < SIZE; ++i) {
        printf("Vvedite %d element massiva: ", i + 1);
        scanf("%d", &X[i]);
    }

    int left = 0;
    int right = SIZE - 1;
    while (left < right) {
        int temp = X[left];
        X[left] = X[right];
        X[right] = temp;
        
        left++;
        right--;
    }
    
    printf("Massiv v obratnom poryadke:\n");
    for (int i = 0; i < 10; ++i) {
        printf("%d ", X[i]);
    }
    printf("\n");
    
    return 0;
}
