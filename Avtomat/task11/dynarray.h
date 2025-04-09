#ifndef _DYNARRAY_H_
#define _DYNARRAY_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int* items;
    size_t size;
    size_t capacity;
} DynArray;

DynArray dynarray_create(size_t capacity);
void dynarray_add(DynArray* array, int item);
bool dynarray_get(DynArray* array, size_t index, int* out_result);
bool dynarray_remove(DynArray* array, size_t index);
bool dynarray_insert(DynArray* array, size_t index, int item);
void dynarray_free(DynArray* array);

#endif