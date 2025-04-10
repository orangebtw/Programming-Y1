#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <Windows.h>
#endif

#define CHAR_T char

typedef struct {
    CHAR_T* name;
    unsigned char day;
    unsigned char month;
    unsigned short year;
} Student;

long long readline(CHAR_T** line, size_t* line_cap, FILE* file) {
    CHAR_T buffer[128];

    if (*line == NULL || *line_cap == 0) {
        *line_cap = 128;
        *line = malloc(*line_cap * sizeof(CHAR_T));
    }

    size_t line_len = 0;
    while (fgets(buffer, 128, file)) {
        size_t buf_len = strlen(buffer);

        if (line_len + buf_len >= *line_cap) {
            *line_cap *= 2;
            *line = realloc(*line, (*line_cap) * sizeof(CHAR_T));
        }

        size_t bytes_to_copy = buf_len;
        if (buffer[buf_len - 1] == '\n') {
            bytes_to_copy -= 1;
        }

        memcpy((*line) + line_len, buffer, bytes_to_copy * sizeof(CHAR_T));
        line_len += bytes_to_copy;

        if (buffer[buf_len - 1] == '\n') {
            (*line)[line_len] = '\0';
            return line_len;
        }
    }

    return -1;
}

int student_comp(const void* arg1, const void* arg2) {
    Student* a = (Student*) arg1;
    Student* b = (Student*) arg2;

    if (a->year > b->year) return 1;
    if (a->year < b->year) return -1;

    if (a->month > b->month) return 1;
    if (a->month < b->month) return -1;

    if (a->day > b->day) return 1;
    if (a->day < b->day) return -1;

    return 0;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    CHAR_T* line = NULL;
    size_t line_cap = 0;

    printf("Enter the number of students: ");

    if (readline(&line, &line_cap, stdin) < 0) {
        printf("ERROR\n");
        return 1;
    }

    size_t count = 0;
    if (sscanf(line, "%zu", &count) <= 0) {
        printf("You didn't provide the number of students.\n");
        return 1;
    }

    if (count == 0) {
        return 0;
    }

    Student* students = malloc(count * sizeof(Student));

    for (size_t i = 0; i < count; ++i) {
        printf("Enter the student data: ");
        if (readline(&line, &line_cap, stdin) < 0) {
            printf("ERRRO\n");
            return 1;
        }

        CHAR_T* word = strtok(line, " ");
        if (word == NULL) {
            printf("ERROR\n");
            return 1;
        }

        size_t wsize = (strlen(word) + 1) * sizeof(CHAR_T);
        CHAR_T* name = malloc(wsize);
        memcpy(name, word, wsize);

        word = strtok(NULL, " ");
        if (word == NULL) {
            printf("ERROR\n");
            return 1;
        }
        
        unsigned char day = strtol(word, NULL, 10);

        word = strtok(NULL, " ");
        if (word == NULL) {
            printf("ERROR\n");
            return 1;
        }

        unsigned char month = strtol(word, NULL, 10);

        word = strtok(NULL, " ");
        if (word == NULL) {
            printf("ERROR\n");
            return 1;
        }

        unsigned short year = strtol(word, NULL, 10);

        students[i] = (Student) {
            .name = name,
            .day = day,
            .month = month,
            .year = year
        };
    }

    free(line);

    qsort(students, count, sizeof(Student), student_comp);

    for (size_t i = 0; i < count; ++i) {
        Student* student = &students[i];
        printf("%s %d %d %d\n", student->name, student->day, student->month, student->year);

        free(student->name);
    }

    free(students);

    return 0;
}