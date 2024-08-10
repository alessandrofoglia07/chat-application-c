#include "client.h"
#include <common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
        char message[BUFFER_SIZE];

        // read message from user
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';

        // exit the loop if the user types "exit"
        if (strcmp(message, "exit") == 0) {
            printf("-- Exiting...\n");
            exit(0);
        }

        // send message to server
        if (send(s, message, strlen(message), 0) < 0) {
            printf("-- Failed to send message\n");
        }
    }
}

void *recvMessages(void *socketDesc) {
    const fd s = *(fd *) socketDesc;
    char buf[BUFFER_SIZE];

    while (1) {
        // Receive message from server
        const int bytesRead = recv(s, buf, BUFFER_SIZE, 0);
        if (bytesRead <= 0) {
            printf("-- Server disconnected.\n");
            break;
        }

        // Null-terminate the received message and print it
        buf[bytesRead] = '\0';
        printf("%s\n", buf);
    }

    return NULL;
}
