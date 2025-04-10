#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    long numerator;
    long denominator;
} Fract;

Fract fract_multiply(Fract a, Fract b) {
    return (Fract) {
        .numerator = a.numerator * b.numerator,
        .denominator = a.denominator * b.denominator
    };
}

Fract fract_divide(Fract a, Fract b) {
    return (Fract) {
        .numerator = a.numerator * b.denominator,
        .denominator = a.denominator * b.numerator
    };
}

Fract fract_plus(Fract a, Fract b) {
    if (a.denominator == b.denominator) {
        return (Fract) {
            .numerator = a.numerator + b.numerator,
            .denominator = a.denominator
        };
    } else {
        return (Fract) {
            .numerator = (a.numerator * b.denominator) + (b.numerator * a.denominator),
            .denominator = a.denominator * b.denominator
        };
    }
}

Fract fract_minus(Fract a, Fract b) {
    if (a.denominator == b.denominator) {
        return (Fract) {
            .numerator = a.numerator - b.numerator,
            .denominator = a.denominator
        };
    } else {
        return (Fract) {
            .numerator = (a.numerator * b.denominator) - (b.numerator * a.denominator),
            .denominator = a.denominator * b.denominator
        };
    }
}

Fract fract_simplify(Fract f) {
    long a = f.numerator;
    long b = f.denominator;

    while (a && b) {
        if (a > b) {
            a %= b;
        } else {
            b %= a;
        }
    }

    long gcd = a + b;

    return (Fract) {
        .numerator = f.numerator / gcd,
        .denominator = f.denominator / gcd
    };
}

typedef enum {
    PLUS = 1,
    MINUS,
    MULTIPLY,
    DIVIDE
} Operation;

typedef enum {
    FRACTION,
    OPERATION
} TokenType;

typedef struct {
    union {
        Fract fraction;
        Operation op;
    };
    TokenType type;
} Token;

Token token_fraction(Fract fraction) {
    return (Token) { .type = FRACTION, .fraction = fraction };
}

Token token_op(Operation op) {
    return (Token) { .type = OPERATION, .op = op };
}

bool parse_fract(char* str, Fract* out_fract) {
    char* s = malloc((strlen(str) + 1) * sizeof(char));
    strcpy(s, str);

    char* num = strtok(s, "/");
    if (num == NULL) return false;

    if (!sscanf(num, "%ld", &out_fract->numerator)) {
        return false;
    }

    num = strtok(NULL, "/");
    if (num == NULL) return false;
    
    if (!sscanf(num, "%ld", &out_fract->denominator)) {
        return false;
    }

    free(s);

    return true;
}

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

#define stack_pop(STACK) (STACK)->items[--((STACK)->size)]

#define vec_free(VEC) do { \
    free((VEC)->items); \
    (VEC)->items = NULL; \
    (VEC)->capacity = 0; \
    (VEC)->size = 0; \
} while (0)

int get_word_count(char* str) {
    int word_count = 0;
    for (; *str != '\0'; ++str) {
        if (isspace(*str)) word_count++;
    }

    return word_count;
}

typedef struct {
    Token* items;
    size_t capacity;
    size_t size;
} Tokens;

typedef struct {
    Operation* items;
    size_t capacity;
    size_t size;
} Operators;

bool parse_tokens(char* str, Tokens* out_tokens) {
    int word_count = get_word_count(str);
    char** words = malloc(word_count * sizeof(char*));

    char* word = strtok(str, " ");
    for (int i = 0; i < word_count; ++i) {
        words[i] = word;
        word = strtok(NULL, " ");
    }

    Tokens tokens = {0};

    bool op = false;
    for (int i = 0; i < word_count; ++i) {
        char* token = words[i];

        if (op) {
            switch (*token) {
                case '*':
                    vec_push(&tokens, token_op(MULTIPLY));
                break;
                case '+':
                    vec_push(&tokens, token_op(PLUS));
                break;
                case '-':
                    vec_push(&tokens, token_op(MINUS));
                break;
                case '/':
                    vec_push(&tokens, token_op(DIVIDE));
                break;
                default: {
                    printf("Syntax error\n");
                    return false;
                }
            }
        } else {
            Fract fract;
            if (parse_fract(token, &fract)) {
                vec_push(&tokens, token_fraction(fract));
            } else {
                printf("Syntax error\n");
                return false;
            }
        }

        op = !op;
    }

    *out_tokens = tokens;

    return true;
}

int get_priority(Operation operation) {
    switch (operation) {
    case PLUS: return 1;
    case MINUS: return 1;
    case MULTIPLY: return 2;
    case DIVIDE: return 2;
    }
}

Operation char_to_op(char c) {
    switch (c) {
        case '*': return MULTIPLY;
        case '/': return DIVIDE;
        case '+': return PLUS;
        case '-': return MINUS;
    }

    return 0;
}

// https://en.wikipedia.org/wiki/Shunting_yard_algorithm
bool shunting_yard(char* str, Tokens* out_tokens) {
    int word_count = get_word_count(str);
    char** words = malloc(word_count * sizeof(char*));

    char* word = strtok(str, " ");
    for (int i = 0; i < word_count; ++i) {
        words[i] = word;
        word = strtok(NULL, " ");
    }

    Tokens tokens = {0};
    Operators operators = {0};

    for (int i = 0; i < word_count; ++i) {
        char* token = words[i];

        Operation op = char_to_op(*token);

        if (op != 0) {
            while (operators.size > 0) {
                Operation op2 = stack_pop(&operators);
                if (get_priority(op) < get_priority(op2)) {
                    vec_push(&tokens, token_op(op2));
                } else {
                    vec_push(&operators, op2);
                    break;
                }
            }

            vec_push(&operators, op);
        } else {
            Fract fract;
            if (parse_fract(token, &fract)) {
                vec_push(&tokens, token_fraction(fract));
            } else {
                printf("Syntax error\n");
                return false;
            }
        }
    }

    while (operators.size > 0) {
        vec_push(&tokens, token_op(stack_pop(&operators)));
    }

    vec_free(&operators);

    *out_tokens = tokens;

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

    Tokens stack = {0};
    for (size_t i = 0; i < tokens.size; ++i) {
        Token token = tokens.items[i];

        if (token.type == OPERATION) {
            Fract b = stack_pop(&stack).fraction;
            Fract a = stack_pop(&stack).fraction;

            if (a.denominator == 0 || b.denominator == 0) {
                printf("ERROR: Division by zero!\n");
                return 1;
            }

            switch (token.op) {
            case PLUS:
                vec_push(&stack, token_fraction(fract_plus(a, b)));
            break;
            case MINUS:
                vec_push(&stack, token_fraction(fract_minus(a, b)));
            break;
            case MULTIPLY:
                vec_push(&stack, token_fraction(fract_multiply(a, b)));
            break;
            case DIVIDE:
                vec_push(&stack, token_fraction(fract_divide(a, b)));
            break;
            }
        } else {
            vec_push(&stack, token);
        }
    }

    if (stack.size != 1) {
        printf("The expression is not correct\n");
        return 1;
    }

    Fract result = fract_simplify(stack.items[0].fraction);

    printf("%ld/%ld", result.numerator, result.denominator);

    return 0;
}