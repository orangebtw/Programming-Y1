#include <stdio.h>

typedef enum {
    MONDAY = 1,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY,
} WeekDay;

int main() {
    printf("%d\n", MONDAY);
    printf("%d\n", TUESDAY);
    printf("%d\n", WEDNESDAY);
    printf("%d\n", THURSDAY);
    printf("%d\n", FRIDAY);
    printf("%d\n", SATURDAY);
    printf("%d\n", SUNDAY);
    
    return 0;
}