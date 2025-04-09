#include <stdio.h>
#include <stdlib.h>

#include "dynarray.h"

// Функция для вывода всех элементов динамического массива
void print_array(DynArray* array) {
    for (size_t i = 0; i < array->size; ++i) {
        int element;
        dynarray_get(array, i, &element);
        printf("%zu. %d\n", i, element);
    }
}

int main() {
    // Инициализируем рандомайзер
    srand(0);

    // Инициализируем динамический массив с емкостью 4
    DynArray array = dynarray_create(4);

    // Добавляем 10 рандомных элементов
    // (специально больше чем емкость, чтобы протестировать автоматическое расширение)
    for (size_t i = 0; i < 10; ++i) {
        dynarray_add(&array, rand() % 200);
    }

    // Выводим исходный массив
    printf("Initial list (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    // Удаляем элемент по индексу 5
    dynarray_remove(&array, 5);

    // Выводим массив после удаления
    printf("The list with the 5th element removed (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    // Ставим элемент 777 на место по индексу 5
    dynarray_insert(&array, 5, 777);

    // Выводим массив после вставки
    printf("The list with the new 777 element added at the 5th index (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    // Добавляем в конец списка элемент 555
    dynarray_add(&array, 555);

    // Выводим массив после добавления
    printf("The list with the new 555 element added at the end (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    // Добавляем несколько элементов в конец списка, чтобы спровоцировать расширение
    dynarray_add(&array, 1);
    dynarray_add(&array, 2);
    dynarray_add(&array, 3);
    dynarray_add(&array, 4);
    dynarray_add(&array, 5);
    dynarray_add(&array, 6);

    // Выводим список после добавления
    printf("The list with the new 6 elements added at the end (%zu, %zu):\n", array.size, array.capacity);
    print_array(&array);

    // Освобождаем память под массив
    dynarray_free(&array);

    // Выводим значения размера, емкости и указателя на память под элементы массива,
    // чтобы убедиться в правильности освобождения ресурсов
    printf("The list after having been freed: (%zu, %zu, %p)\n", array.size, array.capacity, array.items);

    // Возвращаем нулевое значение, сообщая, что программа завершилась без ошибок
    return 0;
}