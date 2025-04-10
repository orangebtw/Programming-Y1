#include <stdio.h>
#include <stdlib.h>

typedef enum {
    CHAR = 0,
    INT,
    FLOAT
} FooType;

typedef struct {
    union {
        char ch;
        int i;
        float f;
    };
    FooType type;
} Foo;

typedef struct {
    Foo* items;
    size_t size;
    size_t capacity;
} DynArray;

DynArray dynarray_create() {
    return (DynArray) {
        .items = malloc(4 * sizeof(Foo)),
        .size = 0,
        .capacity = 4
    };
}

void dynarray_add(DynArray* array, Foo element) {
    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->items = realloc(array->items, array->capacity * sizeof(Foo));
    }

    array->items[array->size++] = element;
}

int main() {
    DynArray array = dynarray_create();

    dynarray_add(&array, (Foo) {
        .ch = 'Q',
        .type = CHAR
    });

    dynarray_add(&array, (Foo) {
        .i = 123,
        .type = INT
    });

    dynarray_add(&array, (Foo) {
        .f = 13.37,
        .type = FLOAT
    });

    dynarray_add(&array, (Foo) {
        .i = 52,
        .type = INT
    });

    for (size_t i = 0; i < array.size; ++i) {
        Foo* foo = &array.items[i];

        switch (foo->type) {
        case CHAR:
            printf("CHAR: %c\n", foo->ch);
        break;
        case INT:
            printf("INT: %d\n", foo->i);
        break;
        case FLOAT:
            printf("FLOAT: %f\n", foo->f);
        break;
        }
    }

    return 0;
}