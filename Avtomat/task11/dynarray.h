// Данная конструкция называется header guard,
// это прием часто используемый программистами на C,
// чтобы избежать повторного включения одного и того же
// заголовочного файла в код и тем самым не допустить
// конфликт имен
#ifndef _DYNARRAY_H_
#define _DYNARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Описание структуры динамического массива из элементов типа int
typedef struct {
    int* items;
    size_t size;
    size_t capacity;
} DynArray;

// Функция для инициализации динамического массива с заданной емкостью
DynArray dynarray_create(size_t capacity);

// Функция для добавления элемента в конец массива
void dynarray_add(DynArray* array, int item);

// Функция для получения элемента по заданному индексу
// Возвращает false, если индекс находится за пределами границ массива
bool dynarray_get(DynArray* array, size_t index, int* out_result);

// Функция для удаления элемента по заданному индексу
// Смещает все элементы после индекса влево
// Возвращает false, если индекс находится за пределами границ массива
bool dynarray_remove(DynArray* array, size_t index);

// Функция для вставки элемента по заданному индексу
// Смещает все элементы после индекса вправо
// Возвращает false, если индекс находится за пределами границ массива
bool dynarray_insert(DynArray* array, size_t index, int item);

// Функция для освобождения ресурсов массива
void dynarray_free(DynArray* array);

#endif