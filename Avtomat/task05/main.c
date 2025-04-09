#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Функция для поиска строки 'query' в строке 'source' и замены ее на строку 'replacement'
char* find_and_replace(const char* source, const char* query, const char* replacement) {
    // Узнаем длину исходной строки
    size_t source_len = strlen(source);
    // Узнаем длину строки поиска
    size_t query_len = strlen(query);
    // Узнаем длину строки замены
    size_t replacement_len = strlen(replacement);

    size_t result_size = (source_len + 1) * sizeof(char);

    // выделяем память под строку результат
    char* result = malloc(result_size);
    // Копируем исходную строку в строку результат
    memcpy(result, source, result_size);

    // Индекс символа в строке результата
    size_t result_i = 0;
    // Индекс символа в исходной строке
    size_t source_i = 0;

    // Перебираем символы в исходной строке
    for (; source_i < source_len;) {
        bool match = true;

        // Перебираем символы в строке поиска
        // и сравниваем каждый с символом в исходной строке
        // если все символы равны значит мы нашли нужную подстроку
        for (size_t j = 0; j < query_len; ++j) {
            if (source_i + j >= source_len) {
                match = false;
                break;
            }

            if (source[source_i + j] != query[j]) {
                match = false;
                break;
            }
        }

        // Если нужная подстрока найдена, копируем в строку результата
        // символы строки замены начиная с индекса result_i,
        // потом копируем все символы из исходной строки в конец строки результата
        // Например:
        // Исходная строка: helloworld
        // Поиск: lowor
        // Замена: qwe
        // Результатом будет: helqweld
        if (match) {
            // Если строка замены + оставшиеся символы в исходной строке не влезут
            // то увеличиваем емкость строки результата до достаточного и перевыделяем память 
            if (result_i + replacement_len + (source_len - source_i) > result_size) {
                result_size = result_size + (replacement_len + source_len - source_i + 1) * sizeof(char);
                result = realloc(result, result_size);
            }

            // Копируем строку замены на текущее место в строке результата
            memcpy(result + result_i * sizeof(char), replacement, replacement_len);
            // Копируем все оставшиеся символы в исходной строке в конец строки результата
            memcpy(result + (result_i + replacement_len) * sizeof(char), source + (source_i + query_len) * sizeof(char), (source_len - source_i) * sizeof(char));
            // Увеличиваем текущий индекс в строке результата на кол-во символов строки замены
            result_i += replacement_len;
        // Если нужная подстрока не найдена, то просто переходим на следующий символ в строке результата
        } else {
            result_i++;
        }

        // Переходим на следующий символ в исходной строке
        source_i++;
    }

    // Возвращаем строку результат
    return result;
}

int main() {
    // Выделяем память под исходную строку
    char* source = malloc(255 * sizeof(char));
    // Выделяем память под строку поиска
    char* query = malloc(255 * sizeof(char));
    // Выделяем память под строку замены
    char* replacement = malloc(255 * sizeof(char));

    // Просим пользователя ввести исходную строку
    printf("Enter source string: ");
    scanf("%s", source);

    // Просим пользователя ввести строку поиска
    printf("Enter the query: ");
    scanf("%s", query);

    // Просим пользователя ввести строку замены
    printf("Enter the replacement: ");
    scanf("%s", replacement);

    // Вызываем нашу функцию и получаем результат
    char* result = find_and_replace(source, query, replacement);
    // Выводим результат
    printf("%s\n", result);

    // Освобождаем память под все строки
    free(source);
    free(query);
    free(replacement);
    free(result);

    // Возвращем нулевое значение, сообщая, что программа завершилась без ошибок
    return 0;
}