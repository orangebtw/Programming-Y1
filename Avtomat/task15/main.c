#include <stdio.h>
#include <stdlib.h>

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
    if (argc < 2) {
        printf("You didn't provide a path to the file.\n");
        return 1;
    }

    // Открываем файл в байтовом режиме чтения 
    FILE* f = fopen(argv[1], "rb");
    // Если не удалось открыть файл, сообщаем об этом пользователю и завершаем программу с ошибкой
    if (f == NULL) {
        printf("Couldn't open the file.\n");
        return 1;
    }

    // Объявляем переменную в которую будет считываться информация из файла
    Record record;
    // Читаем из файла пока не достигнем конца
    while (fread(&record, sizeof(Record), 1, f) == 1) {
        // Выводим данные считанной структуры
        printf("ID: %d, Name: %s, Price: %.2f\n", record.id, record.name, record.price);
    }

    // Закрываем файл
    fclose(f);

    // Возвращаем нулевое значение, сообщая о том, что программа завершилась без ошибок
    return 0;
}