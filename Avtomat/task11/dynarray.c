#include "dynarray.h"

#include <stdlib.h>

DynArray dynarray_create(size_t capacity) {
    DynArray arr = {0};
    // Выделяем память под массив с заданной емкостью
    arr.items = malloc(capacity * sizeof(*arr.items));
    arr.capacity = capacity;
    return arr;
}

void dynarray_add(DynArray* array, int item) {
    // Если кол-во элементов в массиве больше или равно емкость,
    // то увеличиваем емкость в 2 раза и перевыделяем память
    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->items = realloc(array->items, array->capacity * sizeof(*array->items));
    }

    // Добавляем элемент в конец массива
    array->items[array->size++] = item;
}

bool dynarray_get(DynArray* array, size_t index, int* out_result) {
    // Если индекс не находится внутри границ массива
    // Возвращаем false символизируя ошибку
    if (index >= array->size) return false;

    // Присваиваем элемент по заданному индексу пользовательской переменной
    *out_result = array->items[index];

    // Возвращаем true, символизируя успех
    return true;
}

bool dynarray_remove(DynArray* array, size_t index) {
    // Если индекс не находится внутри границ массива
    // Возвращаем false символизируя ошибку
    if (index >= array->size) return false;

    // Перемещаем все элементы правее индекса влево
    // Тем самым удаляя элемент находящийся по индексу
    for (size_t i = index + 1; i < array->size; ++i) {
        array->items[i - 1] = array->items[i];
    }

    // Уменьшаем количество элементов в массиве на 1
    array->size--;

    // Возвращаем true, символизируя успех
    return true;
}

bool dynarray_insert(DynArray* array, size_t index, int item) {
    // Если индекс не находится внутри границ массива
    // Возвращаем false символизируя ошибку
    if (index >= array->size) return false;

    // Если кол-во элементов в массиве больше или равно емкость,
    // то увеличиваем емкость в 2 раза и перевыделяем память
    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->items = realloc(array->items, array->capacity * sizeof(*array->items));
    }
    
    // Увеличиваем кол-во элементов в массиве на 1
    array->size++;

    // Перемещаем все элементы начиная с индекса вправо
    for (size_t i = array->size; i > index; --i) {
        array->items[i] = array->items[i - 1];
    }

    // Устанавливаем элемент по индексу
    array->items[index] = item;

    // Возвращаем true, символизируя успехс
    return true;
}

void dynarray_free(DynArray* array) {
    // Освобождаем память под элементы
    free(array->items);
    array->items = NULL;
    array->size = 0;
    array->capacity = 0;
}
