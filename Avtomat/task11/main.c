#include <stdio.h>
#include <stdlib.h>

#include "dynarray.h"

void print_array(DynArray* array) {
    for (size_t i = 0; i < array->size; ++i) {
        int element;
        dynarray_get(array, i, &element);
        printf("%zu. %d\n", i, element);
    }
}

int main() {
    srand(0);

    DynArray array = dynarray_create(4);

    for (size_t i = 0; i < 10; ++i) {
        dynarray_add(&array, rand() % 200);
    }

    printf("Initial list (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    dynarray_remove(&array, 5);

    printf("The list with the 5th element removed (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    dynarray_insert(&array, 5, 777);

    printf("The list with the new 777 element added at the 5th index (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    dynarray_add(&array, 555);

    printf("The list with the new 555 element added at the end (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    dynarray_add(&array, 1);
    dynarray_add(&array, 2);
    dynarray_add(&array, 3);
    dynarray_add(&array, 4);
    dynarray_add(&array, 5);
    dynarray_add(&array, 6);

    printf("The list with the new 6 elements added at the end (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    dynarray_free(&array);

    printf("The list after having been freed: (%zu, %zu, %p)\n", array.size, array.capacity, array.items);

    return 0;
}