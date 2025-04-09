#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[20];
    float price;
} Record;

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("You didn't provide a path to the file.\n");
        return 1;
    }

    FILE* f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Couldn't open the file.\n");
        return 1;
    }

    Record record;
    while (fread(&record, sizeof(Record), 1, f) == 1) {
        printf("ID: %d, Name: %s, Price: %.2f\n", record.id, record.name, record.price);
    }

    fclose(f);

    return 0;
}