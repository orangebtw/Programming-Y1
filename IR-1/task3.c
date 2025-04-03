#include <stdio.h>

enum Season {
    WINTER,
    SPRING,
    SUMMER,
    AUTUMN
};

int main() {
    unsigned char month;
    printf("Enter the month (1-12): ");
    scanf("%hhu", &month);

    if (month == 12 || month <= 2) {
        printf("Winter\n");
    } else if (month >= 3 && month <= 5) {
        printf("Spring\n");
    } else if (month >= 6 && month <= 8) {
        printf("Summer\n");
    } else if (month >= 9 && month <= 11) {
        printf("Autumn\n");
    }

    return 0;
}