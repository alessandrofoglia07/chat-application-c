#include "client.h"
#include <common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
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
        perror("Failed to allocate memory for client_fd");
        return 1;
    }
    *arg = clientSocket;

    int err = pthread_create(&sendThread, NULL, sendMessages, arg);
    if (err) {
        printf("An error occurred: %d\n", err);
        free(arg);
        return 1;
    }
    err = pthread_create(&recvThread, NULL, recvMessages, arg);
    if (err) {
        printf("An error occurred: %d\n", err);
        free(arg);
        return 1;
    }

    pthread_join(sendThread, NULL);
    pthread_join(recvThread, NULL);

    close(clientSocket);

    return 0;
}