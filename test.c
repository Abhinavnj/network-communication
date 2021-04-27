#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>

#include "linkedlist.h"

#define BUFSIZE 8

// LinkedList* list = malloc(sizeof(LinkedList));
// list->head = malloc(sizeof(Node));
// initList(list, &list->head);

int main(int argc, char const *argv[])
{
    char* valLen = malloc(strlen(argv[1]));
    sprintf(valLen, "%lu", strlen(argv[1]));

    printf("%s\n", valLen);
    
    // int valLenDigits = floor(log10(abs(strlen(argv[1])))) + 1;
    // char* valLenDigitsStr = malloc(valLenDigits);
    // itoa(valLenDigits, valLenDigitsStr, 10);

    // printf("%d %s\n", valLenDigits, valLenDigitsStr);
    
    
    // int nread;
    // char buf[BUFSIZE];

    // size_t messageSize = BUFSIZE;
    // char* message = malloc(messageSize);
    // strcpy(message, "\0");

    // while ((nread = read(0, buf, BUFSIZE)) > 0) {
    //     size_t len = strlen(message);
    //     if (len + nread >= (messageSize - 1)){
    //         messageSize = messageSize * 2;
    //         message = realloc(message, messageSize);
    //     }

    //     memcpy(message + len, buf, nread);
    //     message[len + nread] = '\0';
    // }

    // // GET\n4\nday\n

    // char* messageTokens[4];
    // char* token;
    // token = strtok(message, "\n");
    // messageTokens[0] = token;

    // /* walk through other tokens */
    // int i = 1;
    // while (token != NULL) {
    //     token = strtok(NULL, "\n");
    //     messageTokens[i] = token;
    //     i++;
    // }

    // int rc = EXIT_SUCCESS;

    // char* messageCode = messageTokens[0];
    // int fieldsLength = atoi(messageTokens[1]);
    // if (fieldsLength == 0 && messagesTokens[1][0] != "0"){
    //     //error - BAD
    //     //return;
    // }

    // char* key = messageTokens[2];
    // char* value = messageTokens[3];

    // if (!strcmp("GET", messageCode)) {
    //     if (fieldsLength == strlen(key) + 1){
    //         rc = getNode(&list, &list->head, key, &value);
    //         if (rc) {
    //             //key-value error  - KNF
    //         }
    //         //send value back
    //     } else {
    //         //error  - LEN
    //     }
    // } else if (!strcmp("SET", messageCode)) {
    //     if (fieldsLength == strlen(key) + 1 + strlen(val) + 1){
    //         rc = setNode(&list, &list->head, key, value);
    //         //send value back
    //     } else {
    //         //error  - LEN
    //     }
    // } else if (!strcmp("DEL", messageCode)) {
    //     if (fieldsLength == strlen(key) + 1){
    //         rc = deleteNode(&list, &list->head, key, &value);
    //         if (rc) {
    //             //key-value error  - KNF
    //         }
    //         //send value back
    //     } else {
    //         //error - LEN
    //     }
    // } else {
    //     //error - BAD
    // }

    // free(message);
    

    return 0;
}