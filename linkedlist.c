#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"

#define lock(X) \
        do { \
            int err = pthread_mutex_lock(X); \
            if (err) { \
                perror("lock"); \
                abort(); \
            } \
        } while (0) \

#define unlock(X) \
        do { \
            int err = pthread_mutex_unlock(X); \
            if (err) { \
                perror("unlock"); \
                abort(); \
            } \
        } while (0) \

int initList(LinkedList* list, Node** head) {
    (*head)->key = NULL;
    (*head)->value = NULL;

    pthread_mutex_init(&(list->lock), NULL);
    lock(&(list->lock));
    unlock(&(list->lock));

    return EXIT_SUCCESS;
}

int insertNode(LinkedList** list, Node** head, char* key, char* value) {
    lock(&((*list)->lock));

    Node* newNode = malloc(sizeof(Node));

    int keyLen = strlen(key) + 1;
    newNode->key = malloc(keyLen);
    memcpy(newNode->key, key, keyLen);

    int valLen = strlen(value) + 1;
    newNode->value = malloc(valLen);
    memcpy(newNode->value, value, valLen);

    newNode->next = NULL;

    if ((*head)->key == NULL) {
        *head = newNode;

        unlock(&((*list)->lock));
        return EXIT_SUCCESS;
    }
    else if (strcmp((*head)->key, key) > 0) {
        newNode->next = *head;
        *head = newNode;

        unlock(&((*list)->lock));
        return EXIT_SUCCESS;
    }
    else {
        Node* current = *head;
        Node* prev = *head;
        while (current != NULL){
            if (strcmp(current->key, key) == 0) {
                free(newNode->key);
                free(newNode->value);
                free(newNode);

                unlock(&((*list)->lock));
                return EXIT_FAILURE;
            }
            else if (strcmp(current->key, key) > 0) {
                newNode->next = current;
                prev->next = newNode;

                unlock(&((*list)->lock));
                return EXIT_SUCCESS;
            }
            prev = current;
            current = current->next;
        }
        prev->next = newNode;
    }

	unlock(&((*list)->lock));

    return EXIT_SUCCESS;
}

int setNode(LinkedList** list, Node** head, char* key, char* value) {
	lock(&((*list)->lock));

    Node* ptr = *head;
    while (ptr != NULL) {
        if (strcmp(ptr->key, key) == 0) {
            int valLen = strlen(value) + 1;
            ptr->value = realloc(ptr->value, valLen);
            memcpy(ptr->value, value, valLen);
            
            unlock(&((*list)->lock));
            return EXIT_SUCCESS;
        }
    }

	unlock(&((*list)->lock));

    return EXIT_FAILURE;
}

int deleteNode(LinkedList** list, Node** head, char* key, char** value) {
    lock(&((*list)->lock));

    Node* current = *head;
    Node* prev = *head;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            int valLen = strlen(current->value) + 1;
            *value = malloc(valLen);
            memcpy(*value, current->value, valLen);

            prev->next = current->next;

            free(current->key);
            free(current->value);
            free(current);

            unlock(&((*list)->lock));
            return EXIT_SUCCESS;
        }
        prev = current;
        current = current->next;
    }

	unlock(&((*list)->lock));
    
    return EXIT_FAILURE;
}

void freeList(LinkedList* list, Node* head) {
    Node* tempNode = head;
    while (head != NULL) {
        tempNode = head;
        head = head->next;
        free(tempNode->key);
        free(tempNode->value);
        free(tempNode);
    }

    pthread_mutex_destroy(&(list->lock));
    free(list);
}

void printList(Node* head) {
    Node* ptr = head;
    while (ptr != NULL) {
        printf("%s: %s\t", ptr->key, ptr->value);
        ptr = ptr->next;
    }
    printf("\n");
}
