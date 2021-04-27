#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <pthread.h>

typedef struct Node {
    char* key;
    char* value;
    struct Node* next;
} Node;

typedef struct LinkedList {
    int count;
    Node* head;
    pthread_mutex_t lock;
} LinkedList;

int initList(LinkedList* list, Node** head);
int setNode(LinkedList** list, Node** head, char* key, char* value);
int getNode(LinkedList** list, Node** head, char* key, char** value);
int deleteNode(LinkedList** list, Node** head, char* key, char** value);
void printList(LinkedList* list, Node* head);
void freeList(LinkedList* list, Node* head);

#endif