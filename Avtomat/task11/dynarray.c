#include "dynarray.h"

#include <stdlib.h>

DynArray dynarray_create(size_t capacity) {
    DynArray arr = {0};
    arr.items = malloc(capacity * sizeof(*arr.items));
    arr.capacity = capacity;
    return arr;
}

void dynarray_add(DynArray* array, int item) {
    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->items = realloc(array->items, array->capacity * sizeof(*array->items));
    }

    array->items[array->size++] = item;
}

bool dynarray_get(DynArray* array, size_t index, int* out_result) {
    if (index >= array->size) return false;

    *out_result = array->items[index];

    return true;
}

bool dynarray_remove(DynArray* array, size_t index) {
    if (index >= array->size) return false;

    for (size_t i = index + 1; i < array->size; ++i) {
        array->items[i - 1] = array->items[i];
    }

    array->size--;

    return true;
}

bool dynarray_insert(DynArray* array, size_t index, int item) {
    if (index >= array->size) return false;

    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->items = realloc(array->items, array->capacity * sizeof(*array->items));
    }
    
    array->size++;

    for (size_t i = array->size; i > index; --i) {
        array->items[i] = array->items[i - 1];
    }

    array->items[index] = item;

    return true;
}

void dynarray_free(DynArray* array) {
    free(array->items);
    array->items = NULL;
    array->size = 0;
    array->capacity = 0;
}
