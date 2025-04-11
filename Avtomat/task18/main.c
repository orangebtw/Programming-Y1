#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>

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

    (*line)[line_len] = '\0';

    // Возвращаем -1 если мы не прочитали ни одного символа, сигнализируя о том, что мы дошли до конца файла
    return line_len > 0 ? line_len : -1;
}

int main(int argc, char** argv) {
    // Проверяем, что аргументов достаточно
    // 1 аргумент - путь к программе
    // 2 аргумент - путь к файлу
    if (argc < 2) {
        printf("You didn't provide a path to the file\n");
        return 1;
    }

    // Открываем файл с системой уравнений в текстовом режиме чтения
    FILE* file = fopen(argv[1], "r");
    // Если не удалось открыть - сообщаем пользователю и завершаем программу
    if (file == NULL) {
        printf("Couldn't open the file\n");
        return 1;
    }

    // Прочтенная строка из файла
    char* line;
    // Емкость строки
    size_t line_cap = 0;

    // Читаем первую строку файла
    // Если не удалось прочитать - сообщаем пользователю об ошибке и завершаем программу
    if (readline(&line, &line_cap, file) < 1) {
        printf("Wrong syntax (not enough data)\n");
        fclose(file);
        return 1;
    }

    // Количество строк в файле
    unsigned long n = 0;
    // Пытаемся конвертировать строку в число, 
    // если не удается - сообщаем пользователю об ошибке и завершаем программу
    if (sscanf(line, "%lu", &n) < 1) {
        printf("Wrong syntax (Incorrect format of line count)\n");
        fclose(file);
        return 1;
    }

    // Емкость матрицы коэффициентов
    unsigned long a_cap = 2;
    // Кол-во элементов в матрице коэффициентов
    unsigned long a_size = 0;
    // Матрица коэффициентов
    double* A = malloc(a_cap * sizeof(double));

    // Максимальное кол-во столбцов среди всех строк матрицы
    size_t max_columns = 0;

    // Флаг обозначающий что произошла ошибка
    bool error = false;

    // Читаем n - 1 строк из файла, потому что последняя строка содержит свободные члены
    for (size_t i = 0; i < n - 1; ++i) {
        // Читаем строку, если не удалось сообщаем пользователю об ошибке, освобождаем ресурсы и завершаем программу
        if (readline(&line, &line_cap, file) < 1) {
            printf("Wrong syntax (not enough data)\n");
            free(A);
            fclose(file);
            return 1;
        }

        // Кол-во столбцов в текущей строке
        size_t columns = 0;

        // Текущее слово в строке
        char* word = strtok(line, " ");
        // Перебираем слова в строке пока их не останется или пока не произойдет ошибка
        while (word != NULL && !error) {
            // Пытаемся конвертировать текущее слово (подстроку) в вещественное число
            char* read = word;
            double k = strtod(read, &read);
            // Если не удается - сообщаем об этом пользователю и устанавливаем флаг ошибки
            if (word == read) {
                printf("Wrong syntax (wrong koefficient format)\n");
                error = true;
                break;
            }

            // Если кол-во элементов в матрице больше или равно ее емкости,
            // то увеличиваем емкость в два раза и перевыделяем память
            if (a_size >= a_cap) {
                a_cap *= 2;
                A = realloc(A, a_cap * sizeof(double));
            }

            // Добавляем коэффициент в матрицу
            A[a_size++] = k;

            // Увеличиваем кол-во столбцов на 1
            columns++;

            // Двигаемся к следующему слову в строке если оно есть
            word = strtok(NULL, " ");
        }

        // Если произошла ошибка - принудительно выходим из цикла
        if (error) break;

        // Обновляем максимальное кол-во столбцов
        max_columns = max(max_columns, columns);
    }

    // Убеждаемся, что матрица квадратная
    if (max_columns != (n - 1)) {
        printf("Matrix must be square\n");
        free(A);
        fclose(file);
        return 1;
    }

    // Читаем последнюю строку в файле, которая содержит свободные члены
    // Если не удалось - сообщаем об этом пользователю и завершаем программу
    if (readline(&line, &line_cap, file) < 1) {
        printf("Wrong syntax (Not enough data)\n");
        free(A);
        fclose(file);
        return 1;
    }

    // Емкость массива свободных членов
    size_t b_cap = 2;
    // Кол-во свободных членов
    size_t b_size = 0;
    // Массив свободных членов
    double* B = malloc(b_cap * sizeof(double));

    // Получаем указатель на первое слово в строке
    char* word = strtok(line, " ");

    while (word != NULL) {
        // Пытаемся конвертировать слово (подстроку) в вещественное число
        char* read = word;
        double k = strtod(read, &read);
        // Если не удалось, то сообщаем об этом пользователю и устанавливаем флаг ошибки
        if (word == read) {
            printf("Wrong syntax (wrong koefficient format)\n");
            error = true;
            break;
        }

        // Если кол-во свободных членов в массиве больше или равно его емкости,
        // то увеличиваем емкость в два раза и перевыделяем память
        if (b_size >= b_cap) {
            b_cap *= 2;
            B = realloc(B, b_cap * sizeof(double));
        }

        // Добавляем свободный член в массив
        B[b_size++] = k;

        // Двигаемся к следующему слову, если оно есть
        word = strtok(NULL, " ");
    }

    // Убеждаемся, что кол-во свободных членов равно кол-ву строк матрицы коэффициентов,
    // если нет - сообщаем об этом пользователю и устанавливаем флаг ошибки
    if (b_size != n - 1) {
        printf("Wrong syntax (Wrong number of b koefficients)\n");
        error = true;
    }

    // Если прозошла ошибка - освобождаем ресурсы и завершаем программу
    if (error) {
        free(A);
        free(B);
        fclose(file);
        return 1;
    }

    // Создаем матрицу коэффициентов из полученных данных
    gsl_matrix_view m = gsl_matrix_view_array(A, n - 1, n - 1);
    // Создаем вектор свободных членов из полученных данных
    gsl_vector_view b = gsl_vector_view_array(B, n - 1);

    // Выделяем память под вектор значений неизвестных
    gsl_vector* x = gsl_vector_alloc(n - 1);
    // Выделяем память под матрицу перестановки
    gsl_permutation* p = gsl_permutation_alloc(n - 1);

    // Знак перестановки
    int s;
    // Производим LU разложение матрицы
    // (представляем матрицу A в виде произведения двух матриц, A = LU, где L - нижняя треугольная матрица, а U - верхняя треугольная матрица)
    int res = gsl_linalg_LU_decomp(&m.matrix, p, &s);
    // Если произошла ошибка - сообщаем пользователю, освобождаем ресурсы и завершаем программу
    if (res != GSL_SUCCESS) {
        printf("An error occured\n");
        gsl_permutation_free(p);
        gsl_vector_free(x);
        free(A);
        free(B);
        fclose(file);
    }

    // Решаем систему линейных уравнений
    res = gsl_linalg_LU_solve(&m.matrix, p, &b.vector, x);
    // Если произошла ошибка - сообщаем пользователю, освобождаем ресурсы и завершаем программу
    if (res != GSL_SUCCESS) {
        printf("An error occured\n");
        gsl_permutation_free(p);
        gsl_vector_free(x);
        free(A);
        free(B);
        fclose(file);
    }

    // Выводим значения неизвестных переменных
    for (size_t i = 0; i < x->size; ++i) {
        printf("%.2lf ", gsl_vector_get(x, i));
    }

    // Освобождаем ресурсы
    gsl_permutation_free(p);
    gsl_vector_free(x);

    free(A);
    free(B);
    fclose(file);

    // Возвращаем нулевое значение, сообщая, что программа завершилась без ошибок
    return 0;
}