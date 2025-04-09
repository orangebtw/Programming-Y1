#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/easy.h>

#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

// Функция для записи информации в файл
size_t write_data(void* ptr, size_t size, size_t count, FILE* stream) {
    return fwrite(ptr, size, count, stream);
}

// Функция для проверки существования файла
bool file_exists(const char* path) {
// Если мы на Windows, то используем функцию _access из io.h
#ifdef _WIN32
    return _access(path, 0) == 0;
#else
// Если мы не на Windows, то скорее всего мы на ОС совместимой со стандартом POSIX,
// значит нам доступна функция access из unistd.h
    return access(path, F_OK) == 0;
#endif
}

// Описание структуры динамической строки (типа std::string в C++)
typedef struct {
    char* data;
    size_t size;
    size_t capacity;
} String;

// Функция для создании динамической строки из статической строки
String string_create(const char* initial) {
    size_t len = strlen(initial);

    String str = {0};
    str.data = malloc(len);
    str.size = len;
    str.capacity = len;

    memcpy(str.data, initial, len);

    return str;
}

// Функция для копирования статической строки в динамическую
void string_copy(String* string, const char* initial) {
    size_t len = strlen(initial);

    if (len >= string->capacity) {
        free(string->data);
        string->capacity = len;
        string->data = malloc(string->capacity);;
    }
    
    memcpy(string->data, initial, len);
    string->size = len;
}

// Функция для копирования статической строки в конец динамической строки
void string_add_str(String* string, const char* str) {
    size_t len = strlen(str);

    if (string->size + len >= string->capacity) {
        string->capacity += len;
        string->data = realloc(string->data, string->capacity * sizeof(*string->data));
    }

    for (size_t i = 0; i < len; ++i) {
        string->data[string->size + i] = str[i];
    }

    string->size += len;
}

// Функция для поиска индекса первого символа ch
long long string_find_first(String* string, char ch) {
    long long index = 0;
    bool found = false;
    while (index < string->size) {
        if (string->data[index] == ch) {
            found = true;
            break;
        }
        index++;
    }
    return found ? index : -1;
}

// Функция для перевода динамической строки в привычную для C строку
char* string_to_cstr(String* string) {
    char* str = malloc(string->size + 1);
    memcpy(str, string->data, string->size);
    str[string->size] = '\0';
    return str;
}

// Функция для копирования статической строки в динамическую строку по индексу
bool string_insert_str(String* string, size_t index, const char* str) {
    if (index >= string->size) return false;

    size_t len = strlen(str);

    if (string->size + len >= string->capacity) {
        string->capacity += len;
        string->data = realloc(string->data, string->capacity * sizeof(*string->data));
    }

    for (size_t i = string->size + len; i >= index; --i) {
        string->data[i] = string->data[i - len];
    }

    for (size_t i = 0; i < len; ++i) {
        string->data[index + i] = str[i];
    }

    string->size += len;

    return true;
}

// Функция для освобождения ресурсов динамической стрлки
void string_free(String* string) {
    free(string->data);
    string->data = NULL;
    string->size = 0;
    string->capacity = 0;
}

// Функция для получения имени файла из ссылки
// Эта функция будет добавлять в имя файла число,
// увеличивая его пока получившееся имя не будет уникальным
// (пока файла с таким именем не будет существовать)
char* get_filename_from_url(const char* url) {
    // Получаем индекс последнего слеша
    char* ptr = strrchr(url, '/');

    // Имя файла
    const char* name = NULL;

    // Если найденный слеш это последний символ в строке
    // или если слеша вообще нет, то тогда имя файла это index.html
    if (*(ptr + 1) == '\0' || ptr == NULL) {
        name = "index.html";
    } else {
        name = ++ptr;
    }

    // Ищем символ ? в имени файла (символ ? в ссылке означает начало передачи параметров)
    // нам нужно избавиться от всех последующих символов,
    // потому что система не даст создать файл со специальными символами
    ptr = strchr(name, '?');
    // Если символ ? есть, ставим на его место терминатор
    // таким образом обрезая строку
    if (ptr != NULL) {
        *ptr = '\0';
    }

    // Создаем динамическую строку
    String result = {0};

    // Сразу конвертируем динамическую строку в обычную строку
    // потому что нам нужно проверять существует ли файл с таким именеи
    char* str = string_to_cstr(&result);

    // Число которое добавится в конец имени файла
    size_t i = 1;
    while (file_exists(str)) {
        // Получаем длину строки которая получится
        size_t len = snprintf(NULL, 0, " (%zu)", i);
        // Выделяем память под строку с числом + нул терминатор
        char* number = malloc(len + 1);
        // Форматируем строку с числом и записываем в выделенную память выше
        snprintf(number, len + 1, " (%zu)", i);

        // Копируем исходное имя в динамическую строку
        string_copy(&result, name);

        // Получаем индекс точки (начало расширения файла)
        long long index = string_find_first(&result, '.');

        // Если нашли точку, вставляем строку с числом левее от точки
        if (index >= 0) {
            string_insert_str(&result, index, number);
        } else { // Если точку не нашли, то просто копируем строку с числом в конец
            string_add_str(&result, number);
        }
        
        // Освобождаем память под предыдущее имя файла
        free(str);
        // Обновляем текущее имя файла
        str = string_to_cstr(&result);

        // Увеличиваем число на 1
        i++;
    }

    // Возвращаем конечное имя файла
    return str;
}

// Функция для чтения строк из файла
long long getline(char** line, size_t* line_cap, FILE* file) {
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

CURLcode download_file(CURL* curl, const char* url) {
    // Получаем имя файла из ссылки
    char* filename = get_filename_from_url(url);
    // Открываем файл в байтовом режиме записи
    FILE* file = fopen(filename, "wb");

    // Если не удалось открыть файл освобождаем ресурсы и выходим из функции
    if (file == NULL) {
        free(filename);
        return 0;
    }

    // Устанавливаем путь до файла с сертификатами
    // Оказывается, в новых версиях curl этого можно не делать,
    // потому что он будет использовать системные сертификаты
    // curl_easy_setopt(curl, CURLOPT_CAINFO, "./cacert.pem");
    // Устанавливаем ссылку
    curl_easy_setopt(curl, CURLOPT_URL, url);
    // Устанавливаем функцию записи
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    // Устанавливаем файл в который будет происходить запись
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    // Разрешаем переходить по редиректам
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    // Устанавливаем лимит по количеству редиректов
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);
    // Говорим curl использовать системные сертификаты (только в новых версиях)
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);

    // Выполняем запрос
    CURLcode res = curl_easy_perform(curl);

    // Проверяем статус результата
    // Если все хорошо выводим сообщение об успешной загрузки файла
    if (res == CURLE_OK) {
        printf("Downloaded %s -> %s\n", url, filename);
    } else { // Если что-то пошло не так сообщаем об этом пользователю и выводим ошибку
        printf("Failed %s: %s\n", url, curl_easy_strerror(res));
    }

    // Освобождаем память под название файла
    free(filename);
    // Закрываем файл
    fclose(file);

    // Возвращаем статус результата
    return res;
}

int main(int argc, char** argv) {
    // Проверяем, что кол-во аргументов достаточно
    // 1 аргумент - путь к программе
    // 2 аргумент - путь к файлу
    // ИЛИ
    // 2+ аргументы - ссылки
    if (argc < 2) {
        printf("You must provide a path to a file containing URLs OR list all the URLs in arguments.\n");
        return 1;
    }

    // Инициализируем библиотеку curl
    CURL* curl = curl_easy_init();
    // Если не удалось инициализировать сообщаем об этом пользователю
    // и завершаем программу с кодом ошибки
    if (curl == NULL) {
        printf("Couldn't initialize curl.\n");
        return 1;
    }

    // Если всего аргументов два (либо одна ссылка, либо файл с ссылками)
    if (argc == 2) {
        // Если аргумент содержит :// значит это ссылка
        if (strstr(argv[1], "://") != NULL) {
            // Скачиваем файл по ссылке
            download_file(curl, argv[1]);
        } else { // Иначе это файл
            // Открываем файл
            FILE* f = fopen(argv[1], "r");
            // Если не удалось открыть сообщаем об этом пользователю,
            // освобождаем ресурсы и завершаем программу с кодом ошибки
            if (f == NULL) {
                printf("Couldn't open the file.\n");
                curl_easy_cleanup(curl);
                return 1;
            }

            // Считанная строка
            char* line = NULL;
            // Емкость считанной строки
            size_t line_cap = 0;

            // Читаем строки из файла пока не дойдем до конца
            while (getline(&line, &line_cap, f) >= 0) {
                // Скачиваем файл по ссылке
                download_file(curl, line);
            }

            // Закрываем файл
            fclose(f);
        }
    } else { // Аргументов не два, значит это перечисление ссылок
        for (int i = 1; i < argc; ++i) {
            // Скачиваем файл по ссылке
            download_file(curl, argv[i]);
        }
    }
    
    // Освобождаем ресурсы
    curl_easy_cleanup(curl);

    // Возвращаем нулевое значение, сообщая, что программа завершилась без ошибок
    return 0;
}