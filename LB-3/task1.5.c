#include <stdio.h>
#include <stdlib.h>

typedef struct linked_list {
    struct linked_list* prev;
    struct linked_list* next;
    long value;
} LinkedList;

LinkedList* linked_list_add(LinkedList* list, long value) {
    LinkedList* node = malloc(sizeof(LinkedList));
    node->prev = NULL;
    node->next = NULL;
    node->value = value;

    if (list == NULL) {
        return node;
    }

    LinkedList* prev = NULL;
    LinkedList* curr = list;

    while (curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }

    node->prev = curr;
    curr->next = node;
    
    return list;
}

void traverse_forward(LinkedList* list) {
    LinkedList* curr = list;
    while (curr != NULL) {
        printf("%ld\n", curr->value);
        curr = curr->next;
    }
}

void traverse_backward(LinkedList* list) {    
    LinkedList* curr = list;
    while (curr->next != NULL) {
        curr = curr->next;
    }

    while (curr != NULL) {
        printf("%ld\n", curr->value);
        curr = curr->prev;
    }
}

int main() {
    LinkedList* list = NULL;

    list = linked_list_add(list, 10);
    list = linked_list_add(list, 15);
    list = linked_list_add(list, 23);
    list = linked_list_add(list, 49);
    list = linked_list_add(list, 52);
    list = linked_list_add(list, 69);

    printf("Linked list from the start:\n");
    traverse_forward(list);

    printf("Linked list from the end:\n");
    traverse_backward(list);

    return 0;
}