#include <string.h>
#include <stdbool.h>
#include <tomcrypt/tomcrypt.h>

// Перечисление типов режимов работы программы
typedef enum {
    ENCRYPT = 1,
    DECRYPT
} MODE;

// Размер блока AES шифровщика
#define AES_BLOCK_SIZE 32

// Функция для добавления падинга в зашифрованный файл
// https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS#5_and_PKCS#7
int pkcs7_pad(unsigned char *block, size_t data_len) {
    int pad_len = AES_BLOCK_SIZE - data_len;
    for (int i = data_len; i < AES_BLOCK_SIZE; i++) {
        block[i] = pad_len;
    }
    return pad_len;
}

// Функция для удаления падинга
// https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS#5_and_PKCS#7
int pkcs7_unpad(unsigned char *buffer, size_t *len) {
    if (*len == 0 || *len % AES_BLOCK_SIZE != 0) {
        return -1;
    }

    int pad_len = buffer[*len - 1];
    if (pad_len < 1 || pad_len > AES_BLOCK_SIZE) {
        return -1;
    }

    for (int i = 0; i < pad_len; i++) {
        if (buffer[*len - 1 - i] != pad_len) {
            return -1;
        }
    }

    *len -= pad_len;
    return 0;
}

// Функция для зашифровки файла
bool encrypt(int cipher_id, FILE* input, FILE* output, unsigned char* key, size_t key_size) {
    // Код ошибки библиотке LibTomCrypt
    int err;

    // Пытаемся инициализировать генератор случайных чисел
    // если не получлось - сообщаем пользователю и выходим из функции
    if (register_prng(&sprng_desc) == -1) {
        printf("Couldn't register SPRNG\n");
        return false;
    }

    prng_state prng;
    // Инициализируем генератор случайных чисел
    // если не получилось - сообщаем пользователю и выходим из функции
    if ((err = sprng_start(&prng)) != CRYPT_OK) {
        printf("Couldn't initialize SPRNG: %s\n", error_to_string(err));
        return false;
    }
    
    // Подготавливаем генератор случайных чисел
    // если не получилось - сообщаем пользователю и выходим из функции        
    if ((err = sprng_ready(&prng)) != CRYPT_OK) {
        printf("Couldn't ready SPRNG up: %s\n", error_to_string(err));
        return false;
    }

    // Инициализационный вектор
    unsigned char IV[AES_BLOCK_SIZE];

    // Генерируем случайный вектор инициализации,
    // если не получилось - сообщаем пользователю и выходим из функции
    if (sprng_read(IV, sizeof(IV), &prng) != sizeof(IV)) {
        printf("Failed to generate IV\n");
        return false;
    }

    // Деинициализируем генератор случайных чисел,
    // если не удалось - сообщаем пользователю
    // Не выходим из функции, потому что эта ошибка не помешает нам зашифровать файл
    if ((err = sprng_done(&prng)) != CRYPT_OK) {
        printf("Couldn't done SPRNG: %s\n", error_to_string(err));
    }

    // Сохраняем инициализационный вектор, чтобы потом использовать его для дешифровки
    fwrite(IV, sizeof(unsigned char), sizeof(IV), output);

    symmetric_CBC cbc;
    // Инициализируем шифровщик в режиме CBC
    // если не получилось - сообщаем пользователю и завершаем программу
    if ((err = cbc_start(cipher_id, IV, key, key_size, 0, &cbc)) != CRYPT_OK) {
        printf("Couldn't initialize CBC: %s\n", error_to_string(err));
        return false;
    }

    // Буффер для чтения частей входного файла
    unsigned char buffer[AES_BLOCK_SIZE];
    // Буффер для записи зашифрованной части файла
    unsigned char encrypted[AES_BLOCK_SIZE];
    // Кол-во прочитанных байтов
    size_t read;
    
    // Читаем файл пока не дойдем до конца
    while ((read = fread(buffer, sizeof(unsigned char), sizeof(buffer), input)) == sizeof(buffer)) {
        // Шифруем прочитанную часть файла
        // если не получилось - сообщаем пользователю и завершаем программу
        if ((err = cbc_encrypt(buffer, encrypted, 32, &cbc)) != CRYPT_OK) {
            printf("An error occured while encrypting: %s\n", error_to_string(err));
            return false;
        }
        // Записываем зашифрованную часть в выходной файл
        fwrite(encrypted, sizeof(unsigned char), sizeof(encrypted), output);
    }

    // Обрабатываем последний кусок файла и добавляем паддинг
    if (read > 0 || feof(input)) {
        // Добавляем паддинг в конец буффера
        pkcs7_pad(buffer, read);

        // Зашифровываем оставшуюся информацию
        if ((err = cbc_encrypt(buffer, encrypted, AES_BLOCK_SIZE, &cbc)) != CRYPT_OK) {
            printf("An error occured while encrypting: %s\n", error_to_string(err));
            return false;
        }
        
        // Записываем последнюю зашифрованную часть в выходной файл
        fwrite(encrypted, sizeof(unsigned char), sizeof(encrypted), output);
    }

    // Деинициализируем шифровщик
    // если не получилось - сообщаем пользователю
    if ((err = cbc_done(&cbc)) != CRYPT_OK) {
        printf("Couldn't done CBC: %s\n", error_to_string(err));
    }

    // Возвращаем true, сообщая, что ошибок нет
    return true;
}

bool decrypt(int cipher_id, FILE* input, FILE* output, unsigned char* key, size_t key_size) {
    // Код ошибки библиотке LibTomCrypt
    int err;

    // Инициализационный вектор
    unsigned char IV[AES_BLOCK_SIZE];

    // Получаем инициализационный вектор (первые 32 байта)
    fread(IV, sizeof(unsigned char), sizeof(IV), input);

    symmetric_CBC cbc;
    // Инициализируем шифровщик в режиме CBC
    // если не получилось - сообщаем пользователю и завершаем программу
    if ((err = cbc_start(cipher_id, IV, key, key_size, 0, &cbc)) != CRYPT_OK) {
        printf("Couldn't initialize CBC: %s\n", error_to_string(err));
        return false;
    }

    // Буффер для чтения частей входного файла
    unsigned char buffer[AES_BLOCK_SIZE];
    // Буффер для записи расшифрованной части файла
    unsigned char decrypted[AES_BLOCK_SIZE];
    size_t read;

    // Предыдущая часть входного файла
    unsigned char prev_block[AES_BLOCK_SIZE];
    // Флаг означающий что текущая часть файла - первая
    bool first = true;

    // Читаем файл пока не дойдем до конца
    while ((read = fread(buffer, sizeof(unsigned char), sizeof(buffer), input)) == sizeof(buffer)) {
        // Если сейчас первая часть - копируем ее в буффер предыдущей части,
        // убираем флаг первой части и принудительно идем на следующую итерацию
        if (first) {
            memcpy(prev_block, buffer, sizeof(prev_block));
            first = false;
            continue;
        }

        // Расшифровываем предыдущую часть входного файла
        // если не получилось - сообщаем пользователю и выходим из функции
        if ((err = cbc_decrypt(prev_block, decrypted, AES_BLOCK_SIZE, &cbc)) != CRYPT_OK) {
            printf("An error occured while encrypting: %s\n", error_to_string(err));
            return false;
        }
        // Записываем расшифрованную часть файла в выходной файл
        fwrite(decrypted, sizeof(unsigned char), sizeof(decrypted), output);
        // Обновляем предыдущую часть входного файла
        memcpy(prev_block, buffer, sizeof(prev_block));
    }

    // Обрабатываем последний кусок файла и удаляем паддинг
    if (read > 0 || feof(input)) {
        // Расшифровываем последнюю часть входного файла
        // если не получилось - сообщаем пользователю и выходим из функции
        if ((err = cbc_decrypt(prev_block, decrypted, AES_BLOCK_SIZE, &cbc)) != CRYPT_OK) {
            printf("An error occured while encrypting: %s\n", error_to_string(err));
            return false;
        }

        size_t len = AES_BLOCK_SIZE;
        // Получаем длину расшифрованной части не включая паддинг
        // если произошла ошибка - сообщаем пользователю и выходим из функции
        if (pkcs7_unpad(decrypted, &len) != 0) {
            printf("Padding error\n");
            return false;
        }

        // Записываем последнюю часть в выходной файл
        fwrite(decrypted, sizeof(unsigned char), len, output);
    }

    // Деинициализируем CBC шифровщик
    if ((err = cbc_done(&cbc)) != CRYPT_OK) {
        printf("Couldn't done CBC: %s\n", error_to_string(err));
    }

    // Возвращаем true, сообщая, что ошибок нет
    return true;
}

int main(int argc, char** argv) {
    // Проверяем, что аргументов достаточно
    // 1 аргумент - путь к программе
    // 2 аргумент - режим (шифрование или дешифрование)
    // 3 аргумент - путь к входному файлу
    // 4 аргумент - путь к выходному программе
    // 5 аргумент - пароль
    if (argc < 5) {
        printf("You must provide the mode, a path to the input file, a path to the output file and keypass\n");
        return 1;
    }

    // Определяем режим работы программы (шифрование или дешфрование)
    MODE mode = 0;
    if (strcmp(argv[1], "-e") == 0) {
        mode = ENCRYPT;
    } else if (strcmp(argv[1], "-d") == 0) {
        mode = DECRYPT;
    } else { // Если был передан неправильный аргумент - сообщаем об этом пользователю и завершаем программу
        printf("The mode must be either -e or -d\n");
        return 1;
    }

    // Пароль
    const char* keypass = argv[4];

    // Инициализируем AES шифровщик
    int aes_id = register_cipher(&aes_desc);
    // Если не получилось - сообщаем об этом пользователю и завершаем программу
    if (aes_id == -1) {
        printf("Couldn't register AES cipher\n");
        return 1;
    }

    // Инициализируем SHA-256 хешер
    if (register_hash(&sha256_desc) == -1) {
        printf("Couldn't register SHA-256 hash\n");
        return 1;
    }

    // Зашифрованный пароль
    unsigned char key[32] = {0};

    // Шифруем пароль с помощью алгоритма SHA-256 
    hash_state sha256;
    sha256_init(&sha256);
    sha256_process(&sha256, (unsigned char*) keypass, strlen(keypass));
    sha256_done(&sha256, key);

    // Пытаемся открыть входной файл
    FILE* input = fopen(argv[2], "rb");
    // Если не получилось - сообщаем пользователю и завершаем программу
    if (input == NULL) {
        printf("Couldn't open the input file\n");
        return 1;
    }

    // Пытаемся открыть выходной файл
    FILE* output = fopen(argv[3], "wb");
    // Если не получилось - сообщаем пользователю, закрываем входной файл и завершаем программу
    if (output == NULL) {
        printf("Couldn't open the output file\n");
        fclose(input);
        return 1;
    }

    // Если режим шифровка - шифруем
    if (mode == ENCRYPT) {
        // Шифруем и если успешно - сообщаем пользователю
        if (encrypt(aes_id, input, output, key, sizeof(key))) {
            printf("Encryption complete\n");
        }
    } else  { // Иначе расшифровываем
        // Расшифровываем и если успешно - сообщаем пользователю
        if (decrypt(aes_id, input, output, key, sizeof(key))) {
            printf("Decryption complete\n");
        }
    }

    // Зануляем буфер ключа в целях безопасности
    memset(key, 0, sizeof(key));

    // Закрываем входной и выходной файл
    fclose(input);
    fclose(output);

    // Возвращаем нулевое значения, сообщая, что программа завершилась без ошибок
    return 0;
}