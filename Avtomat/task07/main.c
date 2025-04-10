#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>

#ifdef _WIN32
#include <Windows.h>
#endif

// Описание структуры слова (само слово и его количество в тексте)
typedef struct {
    char* word;
    size_t count;
} Word;

// Функция для чтения строк из файла
long long readline(char** line, size_t* line_cap, FILE* file) {
    // Буффер в который будут читаться куски файла
    char buffer[128];

    // Если указатель на строку равен NULL или емкость строки равна нулю, то
    // выделяем память под считанную строку размером буффера
    // и устанавливаем емкость
    if (*line == NULL || *line_cap == 0) {
        *line_cap = sizeof(buffer);
        *line = malloc(*line_cap);
    }

    // Общий размер считанной конечной строки
    size_t line_len = 0;
    // Читаем куски файла размером буффера, пока не дойдем до конца файла
    while (fgets(buffer, sizeof(buffer), file)) {
        // Получаем длину строки в буффере
        size_t buf_len = strlen(buffer);

        // Если общая длина считанной строки + длина строки в буффере больше или равна емкости конечной строки, то
        // увеличиваем емкость в 2 раза и перевыделяем память под конечную строку
        if (line_len + buf_len >= *line_cap) {
            *line_cap *= 2;
            *line = realloc(*line, *line_cap);
        }

        // Количество байт которые нужно скопировать в конечную строку
        size_t bytes_to_copy = buf_len;
        // Если на конце строки в буффере стоит символ перевода строки
        // уменьшаем количество байт к копированию на один,
        // таким образом символ перевода строки не попадем в конечную строку
        if (buffer[buf_len - 1] == '\n') {
            bytes_to_copy -= 1;
        }

        // Копируем строку из буффера в конец конечной строки
        memcpy((*line) + line_len, buffer, bytes_to_copy);
        line_len += bytes_to_copy;

        // Если на конце строки в буффере стоит символ перевода строки
        // значит мы считали всю строку, поэтому ставим в конец конечной строки
        // терминирующий ноль и возвращаем длину конечной строки
        if (buffer[buf_len - 1] == '\n') {
            (*line)[line_len] = '\0';
            return line_len;
        }
    }

    // Возвращаем -1, сигнализируя о том, что мы дошли до конца файла,
    // но так и не прочитали строку (не нашли символ перевода строки)
    return -1;
}

// Функция для перевода символов в строке в нижний регистр
void lowercase(char* str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; ++i) {
        str[i] = tolower(str[i]);
    }
}

// Функция-компаратор для сортировки слов в алфавитном порядке
int word_cmp(const void* arg1, const void* arg2) {
    const Word* a = (const Word*) arg1;
    const Word* b = (const Word*) arg2;

    return strcmp(a->word, b->word);
}

// Все разделяющие символы
#define DELIMETERS " ,.:;()[]{}!?"

int main(int argc, char** argv) {
    // Проверяем, что аргументов достаточно
    // 1 аргумент - путь к программе 
    // 2 аргумент - путь к файлу с текстом
    if (argc < 2) {
        printf("You didn't provide a path to a file.\n");
        return 1;
    }

    setlocale(LC_ALL, "ru_RU.UTF-8");

// Если мы на Windows, то устанавливаем кодировку вывода на UTF-8,
// чтобы корректно выводить русские символы
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // Открываем файл в текстовом режиме чтения
    FILE* f = fopen(argv[1], "r");
    // Если не удалось открыть файл, то сообщаем об этом пользователю
    // и завершаем программу с кодом ошибки
    if (f == NULL) {
        printf("Couldn't open the file.\n");
        return 1;
    }

    // Емкось массива слов
    size_t words_cap = 10;
    // Количество слов в массиве
    size_t word_count = 0;
    // Выделяем память под массив слов
    Word* words = malloc(words_cap * sizeof(Word));

    // Считываемая строка из файла
    char* line = NULL;
    // Емкость считываемой строки
    size_t line_cap = 0;

    // Считываем строки из файла пока не дойдем до конца файла
    while (readline(&line, &line_cap, f) >= 0) {
        // Разбиваем строку на слова
        char* word = strtok(line, DELIMETERS);

        // Повторяем пока в строке есть слова
        while (word != NULL) {
            // Переводим слово в нижний регистр
            lowercase(word);

            // Если количество слов больше или равно емкость массива слов,
            // то увеличиваем емкость в 2 раза и перевыделяем память
            if (word_count >= words_cap) {
                words_cap *= 2;
                words = realloc(words, words_cap * sizeof(Word));
            }

            // Перебираем все раннее считанные слова и сравниваем их с новым словом
            // Если они оба равны, то увеличиваем количество у существующего слова в массиве
            bool found = false;
            for (size_t i = 0; i < word_count; ++i) {
                Word* w = &words[i];
                if (strcmp(w->word, word) == 0) {
                    w->count++;
                    found = true;
                    break;
                }
            }

            // Если такого слова в массиве нет, добавляем его
            if (!found) {
                // Узнаем длину слова + нул-терминатор
                size_t wsize = strlen(word) + 1;
                // Выделяем память под слово
                char* p = malloc(wsize * sizeof(char));
                // Копируем слово в выделенную память
                memcpy(p, word, wsize * sizeof(char));

                // Добавляем новое слово в массив и увеличиваем количество слов на 1
                words[word_count++] = (Word) {
                    .word = p,
                    .count = 1
                };
            }

            // Получаем следующее слово
            word = strtok(NULL, DELIMETERS);
        }
    }

    // Закрываем файл, т.к. он нам больше не нужен
    fclose(f);

    // Осовобождаем память под строку, т.к. она нам больше не нужна
    free(line);

    // Сортируем слова в алфавитном порядке
    qsort(words, word_count, sizeof(Word), word_cmp);

    // Выводим все слова и их количество
    for (size_t i = 0; i < word_count; ++i) {
        Word* word = &words[i];

        printf("%s : %zu\n", word->word, word->count);

        // Тут же освобождаем память под слово
        free(word->word);
    }

    // Освобождаем память выделенную под массив слов
    free(words);

    // Возвращаем нулевой значение, сообщая, что программа завершилась без ошибок
    return 0;
}