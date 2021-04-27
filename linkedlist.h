#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <pthread.h>

typedef struct Node {
    char* key;
    char* value;
    struct Node* next;
} Node;

typedef struct LinkedList {
    Node* head;
    pthread_mutex_t lock;
} LinkedList;

int initList(LinkedList* list, Node** head);
int insertNode(LinkedList** list, Node** head, char* key, char* value);
int setNode(LinkedList** list, Node** head, char* key, char* value);
int deleteNode(LinkedList** list, Node** head, char* key, char** value);
void printList(Node* head);
void freeList(LinkedList* list, Node* head);

#endif