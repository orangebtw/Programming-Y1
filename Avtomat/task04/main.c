#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Структура дроби
typedef struct {
    long numerator; // Числитель
    long denominator; // Знаменатель
} Fract;

// Функция, перемножающая две дроби и возвращающая резуьтат
Fract fract_multiply(Fract a, Fract b) {
    // Просто умножаем числитель на числитель
    // и знаменатель на знаменатель
    return (Fract) {
        .numerator = a.numerator * b.numerator,
        .denominator = a.denominator * b.denominator
    };
}

// Функция, которая делит две дроби и возвращает результат
Fract fract_divide(Fract a, Fract b) {
    // Переворачиваем вторую дробь, умножая
    // числитель первой дроби на знаменатель второй
    // и знаменатель первой на числитель второй
    return (Fract) {
        .numerator = a.numerator * b.denominator,
        .denominator = a.denominator * b.numerator
    };
}

// Функция, складывающая две дроби и возвращающая результат
Fract fract_plus(Fract a, Fract b) {
    // Если знаменатели одинаковые - просто складываем числители
    if (a.denominator == b.denominator) {
        return (Fract) {
            .numerator = a.numerator + b.numerator,
            .denominator = a.denominator
        };
    } else { // Иначе приводим дроби к общему знаменателю и складываем
        return (Fract) {
            .numerator = (a.numerator * b.denominator) + (b.numerator * a.denominator),
            .denominator = a.denominator * b.denominator
        };
    }
}

// Функция, вычитающая две дроби и возвращающая результат
Fract fract_minus(Fract a, Fract b) {
    // Если знаменатели одинаковые - просто вычитаем числители
    if (a.denominator == b.denominator) {
        return (Fract) {
            .numerator = a.numerator - b.numerator,
            .denominator = a.denominator
        };
    } else { // Иначе приводим дроби к общему знаменателю и вычитаем
        return (Fract) {
            .numerator = (a.numerator * b.denominator) - (b.numerator * a.denominator),
            .denominator = a.denominator * b.denominator
        };
    }
}

// Функция, сокращающая дробь путем деления числителя и знаменателя на НОД,
// который находится используя алгоритм Евклида
Fract fract_simplify(Fract f) {
    // Числитель
    long a = f.numerator;
    // Знаменатель
    long b = f.denominator;

    // Алгоритм Евклида для нахождения НОД
    while (a && b) {
        if (a > b) {
            a %= b;
        } else {
            b %= a;
        }
    }

    // НОД
    long gcd = a + b;

    return (Fract) {
        .numerator = f.numerator / gcd,
        .denominator = f.denominator / gcd
    };
}

// Перечисление типов мат. операций
typedef enum {
    PLUS = 1,
    MINUS,
    MULTIPLY,
    DIVIDE
} Operation;

// Перечисление типов токенов
typedef enum {
    FRACTION,
    OPERATION
} TokenType;

// Описание структуры токена
// Токен - это либо дробь, либо мат. операция
typedef struct {
    union {
        Fract fraction;
        Operation op;
    };
    TokenType type;
} Token;

// Вспомогательная функция, создающая токен дроби, чтобы было удобно
Token token_fraction(Fract fraction) {
    return (Token) { .type = FRACTION, .fraction = fraction };
}

// Вспомогательная функция, создающая токен мат. операции, чтобы было удобно
Token token_op(Operation op) {
    return (Token) { .type = OPERATION, .op = op };
}

// Функция, которая преобразовывает набор символов типа (A/B) в структуру дроби
// Возвращает true в случае успеха
bool parse_fract(char* str, Fract* out_fract) {
    char* s = malloc((strlen(str) + 1) * sizeof(char));
    strcpy(s, str);

    // Разделяем строку по слешу и получаем подстроку левее слеша (числитель)
    char* num = strtok(s, "/");
    if (num == NULL) return false;

    // Пытаемся преобразовать числитель в виде строки в число
    if (!sscanf(num, "%ld", &out_fract->numerator)) {
        return false;
    }

    // Получаем подстроку после слеша (знаменатель)
    num = strtok(NULL, "/");
    if (num == NULL) return false;
    
    // Пытаемся преобразовать знаменатель в виде строки в число
    if (!sscanf(num, "%ld", &out_fract->denominator)) {
        return false;
    }

    // Освобождаем память выделенную под строку
    free(s);

    return true;
}

// Макрос для добавления элемента в динамический массив любого типа
#define vec_push(VEC, ITEM) do { \
    if ((VEC)->size >= (VEC)->capacity) { \
        size_t new_capacity = (VEC)->capacity * 2; \
        if (new_capacity == 0) { \
            new_capacity = 4; \
        } \
        (VEC)->items = realloc((VEC)->items, new_capacity * sizeof(*((VEC)->items))); \
        (VEC)->capacity = new_capacity; \
    } \
    (VEC)->items[(VEC)->size++] = (ITEM); \
} while (0)

// Макрос для вытаскивания последнего элемента из стека
#define stack_pop(STACK) (STACK)->items[--((STACK)->size)]

// Макрос для освобождения ресурсов динамического массива любого типа
#define vec_free(VEC) do { \
    free((VEC)->items); \
    (VEC)->items = NULL; \
    (VEC)->capacity = 0; \
    (VEC)->size = 0; \
} while (0)

// Динамический массив токенов
typedef struct {
    Token* items;
    size_t capacity;
    size_t size;
} Tokens;

// Динамический массив математических операций
typedef struct {
    Operation* items;
    size_t capacity;
    size_t size;
} Operators;

// Функция, которая возвращает приоритет для переданного типа математической операции
int get_priority(Operation operation) {
    switch (operation) {
    case PLUS: return 1;
    case MINUS: return 1;
    case MULTIPLY: return 2;
    case DIVIDE: return 2;
    }
}

// Функция, которая возвращает тип математической операции соответствующей символу
// Если символ не соответствует никакой мат. операции - возвращает 0
Operation char_to_op(char c) {
    switch (c) {
        // Если символ звездочка - операция умножения
        case '*': return MULTIPLY;
        // Если символ слеш - операция деления
        case '/': return DIVIDE;
        // Если символ плюс - операция плюс
        case '+': return PLUS;
        // Если символ минус - операция вычитания
        case '-': return MINUS;
    }

    return 0;
}

// Функция преобразовывающая математическое выражение в виде строку в токены
// в виде Обратной Польской Записи или Reverse Polish Notation (https://en.wikipedia.org/wiki/Reverse_Polish_notation),
// используя Алгоритм Сортировочной Станции или Shunting Yard Algorithm (https://en.wikipedia.org/wiki/Shunting_yard_algorithm)
bool shunting_yard(char* str, Tokens* out_tokens) {
    // Емкость массива слов
    size_t words_cap = 4;
    // Количество слов в массиве
    size_t word_count = 0;
    // Массив слов
    char** words = malloc(words_cap * sizeof(char*));

    // Разделяем строку по пробелам и получаем указатель на первое слово
    char* word = strtok(str, " ");
    // Повторяем пока в строке не останется слов
    while (word != NULL) {
        // Если количество слов превышает емкость массива,
        // то увеличиваем емкость в два раза и перевыделяем память
        if (word_count >= words_cap) {
            words_cap *= 2;
            words = realloc(words, words_cap * sizeof(char*));
        }

        // Добавляем слово в массив
        words[word_count++] = word;
        // Получаем указатель на следующее слово или NULL если оно не существует
        word = strtok(NULL, " ");
    }

    // Динамический массив токенов
    Tokens tokens = {0};
    // Стек мат операций
    Operators operators = {0};

    // Проходимся по всем словам в строке
    for (size_t i = 0; i < word_count; ++i) {
        // Получаем слово из массива
        char* token = words[i];

        // Пытаемся получить тип мат операции по первому символу в слове
        Operation op = char_to_op(*token);

        // Если удалось, значит это мат операция
        if (op != 0) {
            // Сортируем мат операции по приоритету
            // Повторяем пока стеке мат операций не будет пустым
            while (operators.size > 0) {
                // Вытаскиваем последнюю мат операцию из стека
                Operation op2 = stack_pop(&operators);
                // Если приоритет найденой мат операции ниже приоритета мат операции из стека,
                // то перемещаем мат операцию из стека в общий массив токенов
                if (get_priority(op) < get_priority(op2)) {
                    vec_push(&tokens, token_op(op2));
                } else { // Иначе засовываем вытащенную мат операцию обратно в стек мат операций
                    vec_push(&operators, op2);
                    break;
                }
            }

            // Добавляем новую мат операцию в стек
            vec_push(&operators, op);
        } else { // Иначе это дробь
            Fract fract;
            // Пытаемся преобразовать текущее слово в дробь
            if (parse_fract(token, &fract)) {
                // Если удалось - помещаем создаем токен дроби и помещаем его в общий массив токенов
                vec_push(&tokens, token_fraction(fract));
            } else { // Если не удалось - сообщаем, что произошла ошибка и прекращаем работу алгоритма
                printf("Syntax error\n");
                return false;
            }
        }
    }

    // Освобождаем память под массив слов
    free(words);

    // Если в стеке мат операций остались какие-то мат операции - перемещаем их в общий массив
    while (operators.size > 0) {
        vec_push(&tokens, token_op(stack_pop(&operators)));
    }

    // Освобождаем память под стек мат. операций
    vec_free(&operators);

    *out_tokens = tokens;

    // Возвращаем true, сообщая о том, что алгоритм отработал без ошибок 
    return true;
}

int main() {
    // Выделяем память под строку
    char* str = malloc(255 * sizeof(char));

    // Просим у пользователя ввести строку
    printf("Enter an expression: ");
    fgets(str, 255, stdin);

    // Инициализируем динамический список с токенами
    Tokens tokens = {0};
    // Преобразовываем строку в обратную польскую запись или Reverse Polish Notation (https://en.wikipedia.org/wiki/Reverse_Polish_notation),
    // чтобы было удобнее вычислять выражение
    if (!shunting_yard(str, &tokens)) return 1;

    // Освобождаем память под строку, т.к. она нам больше не нужна
    free(str);

    // Стек токенов
    Tokens stack = {0};
    // Перебираем токены
    for (size_t i = 0; i < tokens.size; ++i) {
        // Получаем токен из массива по индексу
        Token token = tokens.items[i];

        // Если тип токена мат операция - производим вычисления
        if (token.type == OPERATION) {
            // Вытаскиваем первую дробь из стека (которая является вторым аргументом)
            Fract b = stack_pop(&stack).fraction;
            // Вытаскиваем вторую дробь из стека (которая является первым аргументов)
            Fract a = stack_pop(&stack).fraction;

            // Если знаменатель хотя бы одной из дроби равен нулю - сообщаем об ошибке и завершаем программу
            if (a.denominator == 0 || b.denominator == 0) {
                printf("ERROR: Division by zero!\n");
                return 1;
            }

            switch (token.op) {
            // Если операция плюс - производим сложение дробей
            case PLUS:
                vec_push(&stack, token_fraction(fract_plus(a, b)));
            break;
            // Если операция минус - производим вычитание дробей
            case MINUS:
                vec_push(&stack, token_fraction(fract_minus(a, b)));
            break;
            // Если операция умножить - производим умножение дробей
            case MULTIPLY:
                vec_push(&stack, token_fraction(fract_multiply(a, b)));
            break;
            // Если операция делить - производим деление дробей
            case DIVIDE:
                vec_push(&stack, token_fraction(fract_divide(a, b)));
            break;
            }
        } else { // Если текущий токен не мат операция (значит это дробь) - добавляем его в стек
            vec_push(&stack, token);
        }
    }

    // По итогу в стеке должен остаться всего один элемент, который является результатом вычисления всего выражения
    // если это не так значит выражение некорректно, тогда сообщаем об ошибке и завершаем программу
    if (stack.size != 1) {
        printf("The expression is not correct\n");
        return 1;
    }

    // Сокращаем дробь-результат, если это возможно
    Fract result = fract_simplify(stack.items[0].fraction);

    // Освобождаем память под стек т.к. он нам больше не нужен
    vec_free(&stack);

    // Выводим результат
    printf("Result: %ld/%ld", result.numerator, result.denominator);

    // Возвращаем нулевое значение, сообщая, что программа завершилась без ошибок
    return 0;
}