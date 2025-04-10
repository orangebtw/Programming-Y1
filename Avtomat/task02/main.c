#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Описание струкуры типа данных Стек типа LIFO (Последним вошел, первым вышел)
typedef struct {
    long* items;
    size_t capacity;
    size_t size;
} Stack;

// Функция для инициализации стека с начальной емкостью в 10 элементов
Stack stack_create() {
    Stack stack = {0};
    stack.items = malloc(10 * sizeof(long));
    stack.capacity = 10;
    stack.size = 0;
    return stack;
};

// Функция для добавления элемента в стек
void stack_push(Stack* stack, long item) {
    // Если текущее количество элементов в стеке больше или равно емкости, то
    // увеличиваем емкость в два раза и перевыделяем пямять под новую емкость
    if (stack->size >= stack->capacity) {
        stack->capacity *= 2;
        stack->items = realloc(stack->items, stack->capacity * sizeof(long));
    }

    // Добавляем элемент в конец
    stack->items[stack->size] = item;
    // Увеличиваем количество элементов на один
    stack->size++;
}

// Функция для вытаскивания последнего элемента из стека
bool stack_pop(Stack* stack, long* out_item) {
    if (stack->size == 0) return false;
    *out_item = stack->items[--stack->size];
    return true;
}

// Функция для освобождения выделенной памяти под элементы стека
void stack_free(Stack* stack) {
    free(stack->items);
    stack->items = NULL;
    stack->capacity = 0;
    stack->size = 0;
}

int main() {
    // Выделяем память под строку, я думаю 255 символов будет достаточно
    char* str = malloc(sizeof(char) * 255);

    // Просим у пользователя ввести строку и записываем ее в выделенную память выше
    printf("Enter an expression: ");
    fgets(str, 255, stdin);

    // Инициализируем стек
    Stack stack = stack_create();

    // Разбиваем строку на слова, разделенные пробелом
    char* token = strtok(str, " ");

    // Флаг для выхода из цикла при возникновении ошибки
    bool error = false;

    // Повторяем пока в строке не останется слов, разделенных пробелом
    while (token != NULL && !error) {
        // Проверяем равен ли текущий символ какому-нибудь математическому символу
        switch (*token) {
            // Если текущий символ звездочка - производим операцию умножения
            case '*': {
                long a, b;
                if (!stack_pop(&stack, &b)) {
                    error = true;
                    break;
                }
                if (!stack_pop(&stack, &a)) {
                    error = true;
                    break;
                }

                stack_push(&stack, a * b);
            } break;
            // Если текущий символ слеш - производим операцию деления
            case '/': {
                long a, b;
                if (!stack_pop(&stack, &b)) {
                    error = true;
                    break;
                }
                if (!stack_pop(&stack, &a)) {
                    error = true;
                    break;
                }

                stack_push(&stack, a / b);
            } break;
            // Если текущий символ плюс - производим операцию сложения
            case '+': {
                long a, b;
                if (!stack_pop(&stack, &b)) {
                    error = true;
                    break;
                }
                if (!stack_pop(&stack, &a)) {
                    error = true;
                    break;
                }

                stack_push(&stack, a + b);
            } break;
            // Если текущий символ минус - производим операцию вычитания
            case '-': {
                long a, b;
                if (!stack_pop(&stack, &b)) {
                    error = true;
                    break;
                }
                if (!stack_pop(&stack, &a)) {
                    error = true;
                    break;
                }

                stack_push(&stack, a - b);
            } break;
            // Если текущий символ не равен никакому математическому символу,
            // значит это число
            default: {
                long number;
                // Конвертируем строку в число и добавляем его в стек
                if (sscanf(token, "%ld", &number) > 0) {
                    stack_push(&stack, number);
                }
            }
        }

        // Двигаемся к следующему слову
        token = strtok(NULL, " ");
    }

    // Освобождаем выделенную память под строку, т. к. она нам больше не нужна
    free(str);

    // Если пользователь ввел выражение правильно, то в конце в стеке должен быть один элемент
    // Если нет или когда известно, что произошла ошибка - сообщаем об этом пользователю
    if (stack.size != 1 || error) {
        printf("The expression is not correct\n");
        // Возвращаем ненулевое значение, означающее, что произошла ошибка
        return 1;
    } else {
        // Достаем из стека единственный оставшийся элемент, который является результатом вычислений
        long result;
        stack_pop(&stack, &result);
        // Выводим пользователю результат
        printf("Result: %ld\n", result);
    }

    // Освобождаем выделенную память под стек
    stack_free(&stack);

    // Возвращаем нулевое значение, означающее, что программа завершилась без ошибок
    return 0;
}