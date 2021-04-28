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

    list->count = 0;

    pthread_mutex_init(&(list->lock), NULL);
    lock(&(list->lock));
    unlock(&(list->lock));

    return EXIT_SUCCESS;
}

int setNode(LinkedList** list, Node** head, char* key, char* value) {
    lock(&((*list)->lock));

    Node* newNode = malloc(sizeof(Node));

    int keyLen = strlen(key) + 1;
    newNode->key = malloc(keyLen);
    memcpy(newNode->key, key, keyLen);

    int valLen = strlen(value) + 1;
    newNode->value = malloc(valLen);
    memcpy(newNode->value, value, valLen);

    newNode->next = NULL;

    if (*head == NULL || (*head)->key == NULL) {
        *head = newNode;
        (*list)->count += 1;

        unlock(&((*list)->lock));
        return EXIT_SUCCESS;
    }
    else if (strcmp((*head)->key, key) > 0) {
        newNode->next = *head;
        *head = newNode;
        (*list)->count += 1;

        unlock(&((*list)->lock));
        return EXIT_SUCCESS;
    }
    else {
        Node* current = *head;
        Node* prev = *head;
        while (current != NULL){
            if (strcmp(current->key, key) == 0) {
                current->value = realloc(current->value, valLen);
                memcpy(current->value, value, valLen);

                free(newNode->key);
                free(newNode->value);
                free(newNode);

                unlock(&((*list)->lock));
                return EXIT_SUCCESS;
            }
            else if (strcmp(current->key, key) > 0) {
                newNode->next = current;
                prev->next = newNode;
                (*list)->count += 1;

                unlock(&((*list)->lock));
                return EXIT_SUCCESS;
            }
            prev = current;
            current = current->next;
        }
        prev->next = newNode;
        (*list)->count += 1;
    }

	unlock(&((*list)->lock));

    return EXIT_SUCCESS;
}

int getNode(LinkedList** list, Node** head, char* key, char** value) {
	lock(&((*list)->lock));

    if ((*list)->count <= 0) {
        unlock(&((*list)->lock));
        return EXIT_FAILURE;
    }

    Node* ptr = *head;
    while (ptr != NULL) {
        if (strcmp(ptr->key, key) == 0) {
            int valLen = strlen(ptr->value) + 1;
            *value = malloc(valLen);
            memcpy(*value, ptr->value, valLen);
            (*value)[valLen - 1] = '\0';

            unlock(&((*list)->lock));
            return EXIT_SUCCESS;
        }
        ptr = ptr->next;
    }

	unlock(&((*list)->lock));

    return EXIT_FAILURE;
}

int deleteNode(LinkedList** list, Node** head, char* key, char** value) {
    lock(&((*list)->lock));

    if ((*list)->count <= 0) {
        unlock(&((*list)->lock));
        return EXIT_FAILURE;
    }

    int count = 0;

    Node* current = *head;
    Node* prev = *head;
    while (current != NULL) {
        count++;
        if (strcmp(current->key, key) == 0) {
            int valLen = strlen(current->value) + 1;
            *value = malloc(valLen);
            memcpy(*value, current->value, valLen);
            (*value)[valLen - 1] = '\0';

            prev->next = current->next;

            if (count == 1) {
                *head = prev->next;
            }

            free(current->key);
            free(current->value);
            free(current);

            (*list)->count -= 1;
            unlock(&((*list)->lock));
            return EXIT_SUCCESS;
        }
        prev = current;
        current = current->next;
    }

	unlock(&((*list)->lock));
    
    return EXIT_FAILURE;
}

void freeList(LinkedList* list, Node** head) {
    Node* tempNode = *head;
    while (head != NULL) {
        tempNode = *head;
        *head = (*head)->next;
        free(tempNode->key);
        free(tempNode->value);
        free(tempNode);
    }

    pthread_mutex_destroy(&(list->lock));
    free(list);
}

void printList(LinkedList* list, Node* head) {
    if ((list)->count == 0){
        return;
    }

    Node* ptr = head;
    while (ptr != NULL) {
        printf("%s: %s\t", ptr->key, ptr->value);
        ptr = ptr->next;
    }
    printf("\n");
}
