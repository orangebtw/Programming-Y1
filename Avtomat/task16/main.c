#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Описание структуры 
typedef struct {
    int id;
    char name[20];
    float price;
} Record;

int main(int argc, char** argv) {
    // Проверяем, что количество аргументов достаточно
    // 1 аргумент - путь к программе
    // 2 аргумент - путь к файлу
    // 3 аргумент - ID записи
    // 4 аргумент - новое имя записи
    // 5 аргумент - новая цена записи
    if (argc < 5) {
        printf("You must provide a path to the file, the id of the record, new name and new price.\n");
        return 1;
    }

    // Пробуем преобразовать ID из строки в число
    char* end = argv[2];
    int id = strtol(argv[2], &end, 10);
    // Если не удалось (указатели одинаковые), выводим сообщение об ошибке и завершаем программу
    if (argv[2] == end) {
        printf("The id of the record must be a number.\n");
        return 1;
    }

    // Пробуем преобразовать новую цену из строки в флоат
    end = argv[4];
    float new_price = strtof(argv[4], &end);
    // Если не удалось (указатели одинаковые), выводим сообщение об ошибке и завершаем программу
    if (argv[4] == end) {
        printf("New price must be a float number.\n");
        return 1;
    }

    // Пробуем открыть файл в байтовом режиме обновления
    FILE* file = fopen(argv[1], "r+b");
    // Если не удалось открыть файл, сообщаем об этом пользователю и завершаем программу
    if (file == NULL) {
        printf("Couldn't open the file.\n");
        return 1;
    }

    // Флаг, означающий, что запись найдена
    bool found = false;

    // Индекс записи
    size_t i = 0;

    // Читаем запись из файла пока не дойдем до конца и сравниваем ее ID, если они совпадают, то
    // устанавливаем флаг и завершаем цикл
    Record record;
    while (fread(&record, sizeof(Record), 1, file) == 1) {
        if (record.id == id) {
            found = true;
            break;
        }

        i++;
    }

    // Копируем новое имя из аргумента в запись
    memcpy(record.name, argv[3], sizeof(record.name));
    // На всякий случай ставим нул терминатор последним символом (если новое имя было больше 19 символов)
    record.name[sizeof(record.name) - 1] = '\0';

    // Копируем новую цену в запись
    record.price = new_price;

    // Если запись не найден сообщаем об этом пользователю и завершаем программу
    if (!found) {
        printf("Record with ID %d not found.\n", id);
        return 1;
    } else {
        // Устанавливаем позицию курсора на первый байт записи которую хотим обновить
        // Если мы на Windows, используем функцию которая умеет работать с большими файлами
        // Если мы не на Windows, то стандартная функция fseek уже умеет работать с большими файлами
        #ifdef _WIN32
            _fseeki64(file, i * sizeof(Record), SEEK_SET);
        #else
            fseek(file, i * sizeof(Record), SEEK_SET);
        #endif

        // Переписываем байты старой записи в файле байтами обновленной
        // Если запись не записалась (произошла ошибка) сообщаем об этом пользователю, закрываем файл
        // и завершаем программу
        if (fwrite(&record, sizeof(Record), 1, file) < 1) {
            printf("An error occured while writing to the file.\n");
            fclose(file);
            return 1;
        } else {
            // Если запись удалась, выводим обновленные данные записи
            printf("Record updated: ID: %d, Name: %s, Price: %f\n", id, record.name, new_price);
        }   
    }

    // Закрываем файл
    fclose(file);

    // Возвращаем нулевое значение, сообщая, что программа завершилась без ошибок
    return 0;
}