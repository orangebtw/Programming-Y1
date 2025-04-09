#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[20];
    float price;
} Record;

int main(int argc, char** argv) {
    if (argc < 5) {
        printf("You must provide a path to the file, the id of the record, new name and new price.\n");
        return 1;
    }

    char* end = argv[2];
    int id = strtol(argv[2], &end, 10);
    if (argv[2] == end) {
        printf("The id of the record must be a number.\n");
        return 1;
    }

    end = argv[4];
    float new_price = strtof(argv[4], &end);
    if (argv[4] == end) {
        printf("New price must be a float number.\n");
        return 1;
    }

    char new_name[20] = {0};
    sscanf(argv[3], "%19s", new_name);

    FILE* file = fopen(argv[1], "r+b");
    if (file == NULL) {
        printf("Couldn't open the file.\n");
        return 1;
    }

    bool found = false;

    size_t i = 0;

    Record record;
    while (fread(&record, sizeof(Record), 1, file) == 1) {
        if (record.id == id) {
            found = true;
            break;
        }

        i++;
    }

    memcpy(record.name, new_name, sizeof(record.name));

    record.price = new_price;

    if (!found) {
        printf("Record with ID %d not found.\n", id);
        return 1;
    } else {
        #ifdef _WIN32
            _fseeki64(file, i * sizeof(Record), SEEK_SET);
        #else
            fseek(file, i * sizeof(Record), SEEK_SET);
        #endif

        if (fwrite(&record, sizeof(Record), 1, file) < 1) {
            printf("An error occured while writing to the file.\n");
            fclose(file);
            return 1;
        } else {
            printf("Record updated: ID: %d, Name: %s, Price: %f\n", id, new_name, new_price);
        }   
    }

    fclose(file);

    return 0;
}