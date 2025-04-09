#include <stdlib.h>
#include <stdio.h>

// Функция которая будет фильтровать значения из массива,
// используя пользовательскую функцию
int* filter(int* arr, size_t n, int (*pred)(int), size_t *out_count) {
    // Емкость отфильтрованного массива
    size_t output_cap = 4;
    // Выделяем память под отфильтрованный массив емкостью в 4 элемента
    int* output = malloc(output_cap * sizeof(int));

    // Перебираем элементы их входного массива
    for (size_t i = 0; i < n; ++i) {
        // Достаем элемент из массива по индексу i
        int element = arr[i];

        // Вызываем пользователюскую функцию, чтобы узнать
        // удовлетворяет ли элемент условию
        if (pred(element)) {
            // Если количество отфильтрованных элементов больше или равно емкости
            // отфильтрованного массива, то увеличиваем емкость в 2 раза
            // и перевыделяем память
            if (*out_count >= output_cap) {
                output_cap *= 2;
                output = realloc(output, output_cap * sizeof(int));
            }
            
            // Добавляем элемент в отфильтрованный массив
            output[*out_count] = element;
            // Увеличиваем количество отфильтрованных элементов на 1
            *out_count += 1;
        }
    }

    // Возвращаем отфильтрованный массив
    return output;
}

// Функция-предикат, которая проверяет элемент на четность
int pred_even(int element) {
    return element % 2 == 0;
}

// Функция-предикат, которая проверяет элемент на отрицательность
int pred_negative(int element) {
    return element < 0;
}

int main() {
    // Количество элементов
    size_t n = 0;

    // Просим пользователя ввести количество элементов
    printf("Enter the number of elements: ");
    scanf("%zu", &n);

    // Выделяем память под массив элементов пользователя
    int* elements = malloc(n * sizeof(int));

    // Считываем элементы пользователя в массив
    printf("Enter the elements:\n");
    for (size_t i = 0; i < n; ++i) {
        scanf("%d", &elements[i]);
    }

    // Количество четных элементов
    size_t even_count = 0;
    // Фильтруем массив элементов с помощью функции проверки на четность
    int* even = filter(elements, n, pred_even, &even_count);

    // Количество отрицательных элементов
    size_t negative_count = 0;
    // Фильтруем массив элементов с помощью функции проверки на отрицательность
    int* negative = filter(elements, n, pred_negative, &negative_count);

    // Выводим четные элементы
    printf("Even:");
    for (size_t i = 0; i < even_count; ++i) {
        printf(" %d", even[i]);
    }
    printf("\n");

    // Выводим отрицательные элементы
    printf("Negative:");
    for (size_t i = 0; i < negative_count; ++i) {
        printf(" %d", negative[i]);
    }
    printf("\n");

    // Освобождаем выделенную память под массив элементов пользователя
    free(elements);
    // Освобождаем выделенную память под массив четных элементов
    free(even);
    // Освобождаем выделенную память под массив отрицательных элементов
    free(negative);

    // Возвращаем нулевое значение, сообщая о том, что программа завершилась без ошибок
    return 0;
}