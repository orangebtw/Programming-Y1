#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sort_float_asc(const void* arg1, const void* arg2) {
    float a = *(const float*) arg1;
    float b = *(const float*) arg2;

    if (a < b) return -1;
    if (a > b) return 1;

    return 0;
}

long long getline(char** line, size_t* line_cap, FILE* file) {
    char buffer[128];

    if (*line == NULL || *line_cap == 0) {
        *line_cap = sizeof(buffer);
        *line = malloc(*line_cap);
    }

    size_t line_len = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        size_t buf_len = strlen(buffer);

        if (line_len + buf_len >= *line_cap) {
            *line_cap *= 2;
            *line = realloc(*line, *line_cap);
        }

        size_t bytes_to_copy = buf_len;
        if (buffer[buf_len - 1] == '\n') {
            bytes_to_copy -= 1;
        }

        memcpy((*line) + line_len, buffer, bytes_to_copy);
        line_len += bytes_to_copy;

        if (buffer[buf_len - 1] == '\n') {
            (*line)[line_len] = '\0';
            return line_len;
        }
    }

    return -1;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("You didn't provide a path to a file.\n");
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Couldn't open the file.\n");
        return 1;
    }

    size_t capacity = 10;
    float* numbers = malloc(capacity * sizeof(float));

    size_t count = 0;

    char* line = NULL;
    size_t line_cap = 0;

    while (getline(&line, &line_cap, f) >= 0) {
        if (sscanf(line, "%f", &numbers[count]) > 0) {
            ++count;

            if (count >= capacity) {
                capacity = capacity * 2;
                numbers = realloc(numbers, capacity * sizeof(float));
            }
        }
    }

    free(line);

    fclose(f);

    qsort(numbers, count, sizeof(float), sort_float_asc);

    float sum = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        sum += numbers[i];
    }

    float mean = sum / count;
    float median = 0.0f;
    if (count % 2 == 0) {
        median = (numbers[count / 2 - 1] + numbers[count / 2]) / 2.0f;
    } else {
        median = numbers[count / 2];
    }

    printf("%.2f %.2f\n", mean, median);
    
    free(numbers);

    return 0;
}