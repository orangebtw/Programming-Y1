#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Перечисление со всеми видами скобок
typedef enum {
    Parenthesis = 0, // (
    SquareBracket, // [
    CurlyBracket, // {
    AngleBracket, // <
} ParenType;

// Описание струкуры типа данных Стек типа LIFO (Последним вошел, первым вышел)
typedef struct {
    ParenType* items;
    size_t capacity;
    size_t size;
} Stack;

// Функция для инициализации стека с начальной емкостью в 10 элементов
Stack stack_create() {
    Stack stack = {0};
    stack.items = malloc(10 * sizeof(ParenType));
    stack.capacity = 10;
    stack.size = 0;
    return stack;
};

// Функция для добавления элемента в стек
void stack_push(Stack* stack, ParenType item) {
    // Если текущее количество элементов в стеке больше или равно емкости, то
    // увеличиваем емкость в два раза и перевыделяем пямять под новую емкость
    if (stack->size >= stack->capacity) {
        stack->capacity *= 2;
        stack->items = realloc(stack->items, stack->capacity * sizeof(ParenType));
    }

    // Добавляем элемент в конец
    stack->items[stack->size] = item;
    // Увеличиваем количество элементов на один
    stack->size++;
}

// Функция для вытаскивания последнего элемента из стека
ParenType stack_pop(Stack* stack) {
    return stack->items[--stack->size];
}

// Функция для проверки пустой ли стек
bool stack_is_empty(Stack* stack) {
    return stack->size == 0;
}

// Функция для освобождения выделенной памяти под элементы стека
void stack_free(Stack* stack) {
    free(stack->items);
    stack->items = NULL;
    stack->capacity = 0;
    stack->size = 0;
}

// Функция для проверки сбалансированы ли скобки в строке
bool balance_parentheses(const char* str) {
    // Инициализируем стек
    Stack stack = stack_create();

    // Проходим по всем символам в строке
    for (; *str != '\0'; ++str) {
        switch (*str) {
            // Когда встречаем открывающуюся скобку - добавляем ее в стек
            case '(': stack_push(&stack, Parenthesis); break;
            case '[': stack_push(&stack, SquareBracket); break;
            case '{': stack_push(&stack, CurlyBracket); break;
            case '<': stack_push(&stack, AngleBracket); break;

            // Когда встречаем закрывающуюся скобку - вытаскиваем последний элемент из стека
            // и сравниваем его с найденной скобкой, они должны быть одинаковы
            case ')': {
                ParenType paren_type = stack_pop(&stack);
                if (paren_type != Parenthesis) {
                    return false;
                }
            } break;
            case ']': {
                ParenType paren_type = stack_pop(&stack);
                if (paren_type != SquareBracket) {
                    return false;
                }
            } break;
            case '}': {
                ParenType paren_type = stack_pop(&stack);
                if (paren_type != CurlyBracket) {
                    return false;
                }
            } break;
            case '>': {
                ParenType paren_type = stack_pop(&stack);
                if (paren_type != AngleBracket) {
                    return false;
                }
            } break;
        }
    }

    // В конце проверяем не осталось ли незакрытых скобок
    bool result = stack_is_empty(&stack);
    
    // Освобождаем выделенную память
    stack_free(&stack);

    return result;
}

int main() {
    // Выделяем память под строку, я думаю 255 символов будет достаточно
    char* str = malloc(255 * sizeof(char));

    // Просим у пользователя ввести строку и записываем ее в выделенную память выше
    printf("Enter any string: ");
    fgets(str, 255, stdin);

    // Проверяем сбалансированы ли скобки
    // если да - выводим YES, если нет выводим NO
    bool result = balance_parentheses(str);
    if (result) {
        printf("YES\n");
    } else {
        printf("NO\n");
    }

    // Освобождаем выделенную память под строку
    free(str);

    // Возвращаем нулевое значение, означающее, что программа завершилась без ошибок
    return 0;
}