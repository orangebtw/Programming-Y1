#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOOK_COUNT 3

typedef struct {
    char title[50];
    char author[50];
    unsigned int year;
} Book;

static void printBook(const Book* book) {
    printf("Book {\n");
    printf("  title: %s,\n", book->title);
    printf("  author: %s,\n", book->author);
    printf("  year: %u,\n", book->year);
    printf("}\n");
}

int main() {
    Book* books = (Book*) malloc(BOOK_COUNT * sizeof(Book));

    for (size_t i = 0; i < BOOK_COUNT; ++i) {
        printf("Enter the title of the %zu book: ", i + 1);
        fgets(books[i].title, sizeof(books[i].title), stdin);
        books[i].title[strcspn(books[i].title, "\r\n")] = 0;

        printf("Enter the author of the %zu book: ", i + 1);
        fgets(books[i].author, sizeof(books[i].author), stdin);
        books[i].author[strcspn(books[i].author, "\r\n")] = 0;

        printf("Enter the release year of the %zu book: ", i + 1);
        scanf("%u", &books[i].year);
        
        fflush(stdin);
    }

    printf("The books:\n");
    for (size_t i = 0; i < BOOK_COUNT; ++i) {
        printBook(&books[i]);
    }
    
    free(books);
}