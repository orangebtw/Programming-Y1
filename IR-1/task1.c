#include <stdio.h>
#include <stdlib.h>

int main() {
    size_t n;
    printf("Enter the number of elements: ");
    scanf("%zu", &n);

    float* array = (float*) malloc(n * sizeof(float));

    for (size_t i = 0; i < n; ++i) {
        printf("Enter the %zu element: ", i + 1);
        scanf("%f", array + i * sizeof(float));
    }

    float sum = 0;
    for (size_t i = 0; i < n; ++i) {
        sum += *(array + i * sizeof(float));
    }

    float average = sum / n;

    printf("Average: %f\n", average);

    free(array);

    return 0;
}