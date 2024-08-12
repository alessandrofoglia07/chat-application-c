#include "client.h"
#include <utils.h>
#include <common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


char name[BUFFER_SIZE];

int main() {
    const fd clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr = {
            .s_addr = inet_addr(SERVER_ADDR)
        }
    };

    connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    struct Request initReq;

    // first request for the client to enter their name
    while (1) {
        printf("Enter name: ");
        fgets(name, BUFFER_SIZE, stdin);
        name[strcspn(name, "\n")] = '\0';


        strcpy(initReq.name, name);
        strcpy(initReq.message, "init");

        sendRequest(clientSocket, &initReq);

        recvRequest(clientSocket, &initReq);

        if (strcmp(initReq.message, "Access Granted.") == 0) {
            break;
        }

        printf("%s: %s\n", initReq.name, initReq.message);
    }

    pthread_t sendThread, recvThread;

    fd *arg = malloc(sizeof(clientSocket));
    if (arg == NULL) {
        perror("-- Failed to allocate memory for client_fd");
        return 1;
    }
    *arg = clientSocket;

    int err = pthread_create(&sendThread, NULL, sendMessages, arg);
    if (err) {
        printf("-- An error occurred: %d\n", err);
        free(arg);
        return 1;
    }
    err = pthread_create(&recvThread, NULL, recvMessages, arg);
    if (err) {
        printf("-- An error occurred: %d\n", err);
        free(arg);
        return 1;
    }

    pthread_join(sendThread, NULL);
    pthread_join(recvThread, NULL);

    free(arg);

    close(clientSocket);

    return 0;
}

void *sendMessages(void *socketDesc) {
    const fd s = *(fd *) socketDesc;

    while (1) {
        char message[MESSAGE_SIZE];

        // read message from user
        fgets(message, MESSAGE_SIZE, stdin);

        message[strcspn(message, "\n")] = '\0';

        // exit the loop if the user types "exit"
        if (strcmp(message, "exit") == 0) {
            printf("-- Exiting...\n");
            exit(0);
        }

        struct Request req;

        strcpy(req.name, name);
        strcpy(req.message, message);

        // Print the formatted message before sending
        printf("%s: %s\n", req.name, req.message);

        // send message to server
        sendRequest(s, &req);
    }
}

void *recvMessages(void *socketDesc) {
    const fd s = *(fd *) socketDesc;
    struct Request req;

    while (1) {
        // Receive message from server
        const int bytesRead = recvRequest(s, &req);
        if (bytesRead == 0) {
            printf("-- Server disconnected\n");
            exit(0);
        }

        printf("%s: %s\n", req.name, req.message);
    }

    return NULL;
}
