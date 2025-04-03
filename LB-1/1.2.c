#include <stdio.h>

int main() {
    int a, b;
    
    printf("Vvedite chislo a: ");
    scanf("%d", &a);
    
    printf("Vvedite chislo b: ");
    scanf("%d", &b);

    int result = a + b;

    printf("%d\n", result);
    
    return 0;
}
