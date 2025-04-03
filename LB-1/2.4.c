#include <stdio.h>

int main() {
    int sum = 0;

    do {
        int num;
        printf("Vvedite chislo: ");
        scanf("%d", &num);

        sum = num / 100 + (num / 10) % 10 + num % 10;
        printf("Summa tsifr: %d\n", sum);
    } while (sum > 10);
    
    return 0;
}
