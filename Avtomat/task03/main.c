#include <stdio.h>
#include <stdlib.h>

// Функция для вычисления факториала
unsigned long long factorial(unsigned long long n) {
    unsigned long long result = 1;
    // Перемножаем все числа от 1 до n
    for (unsigned long long i = 1; i < n; ++i) {
        result *= i;
    }
    return result;
}

// Вспомогательная функция для перевыделения памяти под строку когда ее емкости недостаточно
char* realloc_if_needed(char* line, size_t* line_cap, size_t length) {
    char* new_line = line;
    // Если новая длина строки больше или равна емкости строки, то
    // перевыделяем память под новую длину строки
    if (length >= *line_cap) {
        *line_cap = length + 1; // +1 из-за нулл терминатора
        new_line = realloc(new_line, *line_cap * sizeof(char));
    }
    return new_line;
}

int main() {
    // Кол-во строк треугольника Паскаля
    unsigned long long n = 0;

    // Просим пользователя ввести количество строк
    printf("Enter n: ");
    scanf("%llu", &n);

    // Емкость массива строк
    size_t lines_cap = 2;
    // Кол-во строк в массиве
    size_t lines_count = 0;
    // Динамический массив строк
    char** lines = malloc(lines_cap * sizeof(char*));

    // Генерируем n строк
    for (unsigned long long i = 0; i < n; ++i) {
        // Если кол-во строк в массиве больше или равно емкости,
        // то увеличиваем емкость и перевыделяем память
        if (lines_count >= lines_cap) {
            lines_cap *= 2;
            lines = realloc(lines, lines_cap * sizeof(char*));
        }

        // Емкость строки
        size_t line_cap = 4;
        // Длина строки
        size_t line_length = 0;
        // Строка
        char* line = malloc(line_cap * sizeof(char));

        // Кол-во пробелов перед первым число в строке
        size_t num_spaces = (n - i) * 3;
        // Перевыделяем память, чтобы вместить пробелы если необходимо
        line = realloc_if_needed(line, &line_cap, line_length + num_spaces);

        // Добавляем пробелы в начало строки
        for (size_t q = 0; q < num_spaces; ++q) {
            line[line_length++] = ' ';
        }

        // m = кол-во чисел в строке
        for (unsigned long long m = 0; m < i + 1; ++m) {
            // Число которое добавится
            unsigned long long num = 0;

            // Если число первое или последнее - ставим 1
            if (m == 0 || m == i) {
                num = 1;
            // Если число второе или предпоследнее - ставим номер строки
            } else if (m == 1 || m == i - 1) {
                num = i;
            // Если число третье - вычисляем треугольное число 
            } else if (m == 2) {
                num = (i * (i - 1)) / 2;
            // Если число четвертое - вычисляем тетраэдрическое число
            } else if (m == 3) {
                num = (i * (i + 1) * (i + 2)) / 6;
            // Любое последующее число равно биноминальному коэффициенту
            } else {
                num = factorial(i) / (factorial(m) * factorial(i - m));
            }

            // Узнаем длину подстроки которая добавится в конечную строку
            size_t l = snprintf(NULL, 0, "%6llu", num);
            // Перевыделяем память чтобы вместить новую подстроку если необходимо
            line = realloc_if_needed(line, &line_cap, line_length + l);
            // Добавляем новую подстроку в конец конечной строки
            snprintf(line + line_length, line_cap, "%6llu", num);
            // Увеличиваем общую длину на длину подстроки
            line_length += l;
        }

        // Добавляем новую строку в массив
        lines[lines_count++] = line;
    }

    // Выводим все строки в массиве
    for (size_t i = 0; i < lines_count; ++i) {
        printf("%s\n", lines[i]);
        // Тут же освобождаем память под строку
        free(lines[i]);
    }

    // Освобождаем память под массив строк
    free(lines);

    // Возвращаем нулевое число, сообщая, что программа завершилась без ошибок
    return 0;
}