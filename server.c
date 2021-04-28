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

#include "linkedlist.h"

#define BACKLOG 5

int running = 1;

// the argument we will pass to the connection-handler threads
struct connection {
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
};

LinkedList* list;

int server(char *port);
void *echo(void *arg);
int clientRequest(char* message, int client_fd);
int tokenizeMessage(char* message, char** messageTokens);
char* constructResponse(char* responseCode, char* value);
int sendResponse(int client_fd, char* response);

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s [port]\n", argv[0]);
		return EXIT_FAILURE;
	}

    list = malloc(sizeof(LinkedList));
    list->head = malloc(sizeof(Node));
    initList(list, &list->head);

    server(argv[1]);

    freeList(list, list->head);

    return EXIT_SUCCESS;
}

void handler(int signal)
{
	running = 0;
}


int server(char *port)
{
    struct addrinfo hint, *info_list, *info;
    struct connection *con;
    int error, sfd;
    pthread_t tid;

    // initialize hints
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    // get socket and address info for listening port
    error = getaddrinfo(NULL, port, &hint, &info_list);
    if (error != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    // attempt to create socket
    for (info = info_list; info != NULL; info = info->ai_next) {
        sfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        
        // if we couldn't create the socket, try the next method
        if (sfd == -1) {
            continue;
        }

        // if we were able to create the socket, try to set it up for
        // incoming connections;
        // 
        // note that this requires two steps:
        // - bind associates the socket with the specified port on the local host
        // - listen sets up a queue for incoming connections and allows us to use accept
        if ((bind(sfd, info->ai_addr, info->ai_addrlen) == 0) &&
            (listen(sfd, BACKLOG) == 0)) {
            break;
        }

        // unable to set it up, so try the next method
        close(sfd);
    }

    if (info == NULL) {
        // we reached the end of result without successfuly binding a socket
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    // socket set up at this point
    freeaddrinfo(info_list);

	struct sigaction act;
	act.sa_handler = handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);
	
	sigset_t mask;
	
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);


    // at this point sfd is bound and listening
    printf("Waiting for connection\n");
	while (running) {
    	// create argument struct for child thread
		con = malloc(sizeof(struct connection));
        con->addr_len = sizeof(struct sockaddr_storage);
        	// addr_len is a read/write parameter to accept
        	// we set the initial value, saying how much space is available
        	// after the call to accept, this field will contain the actual address length
        
        // wait for an incoming connection
        con->fd = accept(sfd, (struct sockaddr *) &con->addr, &con->addr_len);
        	// we provide
        	// sfd - the listening socket
        	// &con->addr - a location to write the address of the remote host
        	// &con->addr_len - a location to write the length of the address
        	//
        	// accept will block until a remote host tries to connect
        	// it returns a new socket that can be used to communicate with the remote
        	// host, and writes the address of the remote host into the provided location
        
        // if we got back -1, it means something went wrong
        if (con->fd == -1) {
            perror("accept");
            continue;
        }
        
        // temporarily block SIGINT (child will inherit mask)
        error = pthread_sigmask(SIG_BLOCK, &mask, NULL);
        if (error != 0) {
        	fprintf(stderr, "sigmask: %s\n", strerror(error));
        	abort();
        }

		// spin off a worker thread to handle the remote connection
        error = pthread_create(&tid, NULL, echo, con);

		// if we couldn't spin off the thread, clean up and wait for another connection
        if (error != 0) {
            fprintf(stderr, "Unable to create thread: %d\n", error);
            close(con->fd);
            free(con);
            continue;
        }

		// otherwise, detach the thread and wait for the next connection request
        pthread_detach(tid);

        // unblock SIGINT
        error = pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
        if (error != 0) {
        	fprintf(stderr, "sigmask: %s\n", strerror(error));
        	abort();
        }

    }

	puts("No longer listening.");
	pthread_detach(pthread_self());
	pthread_exit(NULL);

    // never reach here
    return 0;
}

#define BUFSIZE 8

void *echo(void *arg)
{
    char host[100], port[10];
    struct connection *c = (struct connection *) arg;
    int error, nread;

	// find out the name and port of the remote host
    error = getnameinfo((struct sockaddr *) &c->addr, c->addr_len, host, 100, port, 10, NI_NUMERICSERV);
    	// we provide:
    	// the address and its length
    	// a buffer to write the host name, and its length
    	// a buffer to write the port (as a string), and its length
    	// flags, in this case saying that we want the port as a number, not a service name
    if (error != 0) {
        fprintf(stderr, "getnameinfo: %s", gai_strerror(error));
        close(c->fd);
        return NULL;
    }

    printf("[%s:%s] connection\n", host, port);

    char buf[BUFSIZE];

    size_t messageSize = BUFSIZE;
    char* message = malloc(messageSize);
    strcpy(message, "\0");

    while ((nread = read(c->fd, buf, BUFSIZE)) > 0) {
        size_t len = strlen(message);
        if (len + nread >= (messageSize - 1)) {
            messageSize = messageSize * 2;
            message = realloc(message, messageSize);
        }

        memcpy(message + len, buf, nread);
        message[len + nread] = '\0';

        // printf("%s\n", message);
    }

    int rc = clientRequest(message, c->fd); //TODO: if this is failure, close the thing

    free(message);

    printf("[%s:%s] got EOF\n", host, port);

    close(c->fd);
    free(c);
    return NULL;
}

int clientRequest(char* message, int client_fd) {
    int rc = EXIT_SUCCESS;
    
    char* messageTokens[4];
    tokenizeMessage(message, messageTokens);
    
    char* messageCode = messageTokens[0];
    int fieldsLength = atoi(messageTokens[1]);
    if (fieldsLength == 0 && messageTokens[1][0] != '0'){ //TODO: check
        sendResponse(client_fd, "ERR\nBAD\n");
        rc = EXIT_FAILURE;
        return rc;
    }
    char* key = messageTokens[2];
    char* value = messageTokens[3];

    if (!strcmp("GET", messageCode)) {
        if (fieldsLength == strlen(key) + 1){
            rc = getNode(&list, &list->head, key, &value);
            if (rc) {
                sendResponse(client_fd, "KNF\n");
            } else {
                char* response = constructResponse("OKG", value);
                sendResponse(client_fd, response);

                free(response);
            }
        } else {
            sendResponse(client_fd, "ERR\nLEN\n");
            rc = EXIT_FAILURE;
            return rc;
        }
    } else if (!strcmp("SET", messageCode)) {
        if (fieldsLength == strlen(key) + 1 + strlen(value) + 1){
            rc = setNode(&list, &list->head, key, value);
            sendResponse(client_fd, "OKS\n");
        } else {
            sendResponse(client_fd, "ERR\nLEN\n");
            rc = EXIT_FAILURE;
            return rc;
        }
    } else if (!strcmp("DEL", messageCode)) {
        if (fieldsLength == strlen(key) + 1){
            rc = deleteNode(&list, &list->head, key, &value);
            if (rc) {
                sendResponse(client_fd, "KNF\n");
            } else {
                char* response = constructResponse("OKD", value);
                sendResponse(client_fd, response);

                free(response);
            }
        } else {
            sendResponse(client_fd, "ERR\nLEN\n");
            rc = EXIT_FAILURE;
            return rc;
        }
    } else {
        sendResponse(client_fd, "ERR\nBAD\n");
        rc = EXIT_FAILURE;
        return rc;
    }
    
    return rc;
}

int tokenizeMessage(char* message, char** messageTokens){
    char* token;
    int i = 0;

    token = strtok(message, "\n");
    messageTokens[i] = token;
    i++;
    
    while (token != NULL && i < 4) {
        token = strtok(NULL, "\n");
        messageTokens[i] = token;
        i++;
    }

    return EXIT_SUCCESS;
}

// used to construct get and del responses
char* constructResponse(char* responseCode, char* value){
    char* valLen = malloc(strlen(value));
    sprintf(valLen, "%lu", strlen(value) + 1);

    int responseLen = strlen(responseCode) + 1 + strlen(valLen) + 1 + strlen(value) + 1 + 1;
    int usedLen = 0;

    char* response = malloc(responseLen * sizeof(char));
    memcpy(response + usedLen, responseCode, strlen(responseCode));
    usedLen += strlen(responseCode);
    memcpy(response + usedLen, "\n", strlen("\n"));
    usedLen += strlen("\n");
    memcpy(response + usedLen, valLen, strlen(valLen));
    usedLen += strlen(valLen);
    memcpy(response + usedLen, "\n", strlen("\n"));
    usedLen += strlen("\n");
    memcpy(response + usedLen, value, strlen(value));
    usedLen += strlen(value);
    memcpy(response + usedLen, "\n", strlen("\n"));
    usedLen += strlen("\n");
    response[usedLen] = '\0';

    return response;
}

int sendResponse(int client_fd, char* response) {
    write(client_fd, response, strlen(response));

    return EXIT_SUCCESS;
}
