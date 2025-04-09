#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char* find_and_replace(const char* source, const char* query, const char* replacement) {
    size_t source_len = strlen(source);
    size_t query_len = strlen(query);
    size_t replacement_len = strlen(replacement);

    size_t result_size = (source_len + 1) * sizeof(char);

    char* result = malloc(result_size);
    strcpy(result, source);

    size_t result_i = 0;
    size_t source_i = 0;

    for (; source_i < source_len;) {
        bool match = true;

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

        if (match) {
            if (result_i + replacement_len + (source_len - source_i) > result_size) {
                result_size = result_size + (replacement_len + source_len - source_i + 1) * sizeof(char);
                result = realloc(result, result_size);
            }

            memcpy(result + result_i * sizeof(char), replacement, replacement_len);
            memcpy(result + (result_i + replacement_len) * sizeof(char), source + (source_i + query_len) * sizeof(char), (source_len - source_i) * sizeof(char));
            result_i += replacement_len;
        } else {
            result_i++;
        }

        source_i++;
    }

    return result;
}

int main() {
    char* source = malloc(255 * sizeof(char));
    char* query = malloc(255 * sizeof(char));
    char* replacement = malloc(255 * sizeof(char));

    printf("Enter source string: ");
    scanf("%s", source);

    printf("Enter the query: ");
    scanf("%s", query);

    printf("Enter the replacement: ");
    scanf("%s", replacement);

    char* result = find_and_replace(source, query, replacement);
    printf("%s\n", result);

    free(source);
    free(query);
    free(replacement);
    free(result);

    return 0;
}