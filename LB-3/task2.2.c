#include <stdio.h>

typedef union {
    unsigned long number;
    unsigned char bytes[sizeof(unsigned long)];
} U;

int main() {
    U uni = { .number = 12345678 };

    printf("Number: %ld\n", uni.number);

    printf("Bytes:");
    for (size_t i = 0; i < sizeof(uni.bytes); ++i) {
        printf(" 0x%x", uni.bytes[i]);
    }
    printf("\n");

    return 0;
}