#include <stdio.h>

typedef struct {
    unsigned int day : 5;
    unsigned int month : 4;
    unsigned int year;
} Date;

int main() {
    Date d1 = { 23, 12, 2005 };
    Date d2 = { 31, 5, 2006 };

    printf("d1 = %u/%u/%u\n", d1.day, d1.month, d1.year);
    printf("d2 = %u/%u/%u\n", d2.day, d2.month, d2.year);

    return 0;
}