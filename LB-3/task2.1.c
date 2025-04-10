#include <stdio.h>

typedef union {
    char a;
    int b;
    float c;
} SomeUnion;

void print_union(SomeUnion* u) {
    printf("%c, %d, %f\n", u->a, u->b, u->c);
}

int main() {
    SomeUnion a = { .a = 'a' };
    SomeUnion b = { .b = 52 };
    SomeUnion c = { .c = 13.37f };

    print_union(&a);
    print_union(&b);
    print_union(&c);

    return 0;
}