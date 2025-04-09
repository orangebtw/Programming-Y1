#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        if (buffer[buf_len - 1] == '\n' || line_len > 0) {
            (*line)[line_len] = '\0';
            return line_len;
        }
    }

    return -1;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("You didn't provide files.\n");
        return 1;
    }

    FILE* f1 = fopen(argv[1], "r");
    if (f1 == NULL) {
        printf("Couldn't open file: \"%s\"", argv[1]);
        return 1;
    }

    FILE* f2 = fopen(argv[2], "r");
    if (f2 == NULL) {
        printf("Couldn't open file: \"%s\"", argv[2]);
        return 1;
    }

    char* line = NULL;
    size_t line_cap = 0;

    size_t m1_r = 0;
    size_t m1_c = 0;
    size_t m2_r = 0;
    size_t m2_c = 0;

    if (getline(&line, &line_cap, f1) < 0) {
        printf("The file is incorrect.\n");
        return 1;
    }

    sscanf(line, "%zu %zu", &m1_r, &m1_c);

    if (getline(&line, &line_cap, f2) < 0) {
        printf("The file is incorrect.\n");
        return 1;
    }

    sscanf(line, "%zu %zu", &m2_r, &m2_c);

    if (m1_c != m2_r) {
        printf("Matrices cannot be multiplied.\n");
        return 1;
    }

    float* m1 = malloc(m1_r * m1_c * sizeof(float));
    float* m2 = malloc(m2_r * m2_c * sizeof(float));

    for (size_t i = 0; i < m1_r; ++i) {
        if (getline(&line, &line_cap, f1) < 0) {
            printf("Matrix is wrong.\n");
            return 1;
        }

        char* read = line;
        for (size_t j = 0; j < m1_c; ++j) {
            m1[i * m1_c + j] = strtof(read, &read);
        }
    }

    for (size_t i = 0; i < m2_r; ++i) {
        if (getline(&line, &line_cap, f2) < 0) {
            printf("Matrix is wrong.\n");
            return 1;
        }

        char* read = line;  
        for (size_t j = 0; j < m2_c; ++j) {
            m2[i * m2_c + j] = strtof(read, &read);
        }
    }

    free(line);

    fclose(f1);
    fclose(f2);

    size_t n = m1_r;
    size_t m = m1_c;
    size_t p = m2_c;

    float* result = malloc(n * p * sizeof(float));

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < p; ++j) {
            result[i * p + j] = 0;
            for (size_t k = 0; k < m; ++k) {
                result[i * p + j] += m1[i * m + k] * m2[k * p + j];
            }
        }
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < p; ++j) {
            printf("%.2f ", result[i * p + j]);
        }
        printf("\n");
    }

    return 0;
}