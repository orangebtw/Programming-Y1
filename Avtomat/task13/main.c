#include <stdlib.h>
#include <stdio.h>

int* filter(int* arr, size_t n, int (*pred)(int), size_t *out_count) {
    size_t output_cap = 4;
    int* output = malloc(output_cap * sizeof(int));

    for (size_t i = 0; i < n; ++i) {
        int element = arr[i];

        if (pred(element)) {
            if (*out_count >= output_cap) {
                output_cap *= 2;
                output = realloc(output, output_cap * sizeof(int));
            }

            output[*out_count] = element;
            *out_count += 1;
        }
    }

    return output;
}

int pred_even(int element) {
    return element % 2 == 0;
}

int pred_negative(int element) {
    return element < 0;
}

int main() {
    size_t n = 0;

    printf("Enter the number of elements: ");
    scanf("%zu", &n);

    int* elements = malloc(n * sizeof(int));

    printf("Enter the elements:\n");
    for (size_t i = 0; i < n; ++i) {
        scanf("%d", &elements[i]);
    }

    size_t even_count = 0;
    int* even = filter(elements, n, pred_even, &even_count);

    size_t negative_count = 0;
    int* negative = filter(elements, n, pred_negative, &negative_count);

    printf("Even:");
    for (size_t i = 0; i < even_count; ++i) {
        printf(" %d", even[i]);
    }
    printf("\n");

    printf("Negative:");
    for (size_t i = 0; i < negative_count; ++i) {
        printf(" %d", negative[i]);
    }
    printf("\n");

    free(elements);
    free(even);
    free(negative);

    return 0;
}