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

typedef struct linked_list {
    long value;
    struct linked_list* next;
} LinkedList;

void linked_list_print(LinkedList* head) {
    if (head == NULL) return;

    while (head != NULL) {
        printf("%ld\n", head->value);
        head = head->next;
    }
}

LinkedList* linked_list_add_sorted(LinkedList* list, long value) {
    LinkedList* node = malloc(sizeof(LinkedList));
    node->value = value;
    node->next = NULL;

    LinkedList* head = list;
    LinkedList** pos = &list;
    while (head != NULL && head->value < value) {
        pos = &head->next;
        head = head->next;
    }
    *pos = node;
    node->next = head;

    return list;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("You didn't provide the file.\n");
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Couldn't open the file.\n");
        return 1;
    }

    LinkedList* list = {0};

    char* line = NULL;
    size_t line_cap = 0;
    while (getline(&line, &line_cap, f) >= 0) {
        char* read = line;
        long v = strtol(read, &read, 10);

        list = linked_list_add_sorted(list, v);
    }

    linked_list_print(list);

    free(line);
    fclose(f);

    return 0;
}