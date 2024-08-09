#include "server.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const int s = socket(AF_INET, SOCK_STREAM, 0);

    if (s < 0) {
        perror("socket");
        return 1;
    }

    const struct sockaddr_in addr = {
        AF_INET,
        htons(PORT),
        INADDR_ANY
    };

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        close(s);
        return 1;
    }

    if (listen(s, MAX_CONNECTIONS) < 0) {
        perror("listen");
        close(s);
        return 1;
    }

    pthread_t serverInputThread;
    const int serverInputThreadError = pthread_create(&serverInputThread, NULL, ask_for_input, NULL);
    if (serverInputThreadError) {
        printf("An error occurred: %d\n", serverInputThreadError);
        return 1;
    }

    while (1) {
        const int client_fd = accept(s, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        pthread_t client_thread;

        int *arg = malloc(sizeof(client_fd));
        if (arg == NULL) {
            perror("Failed to allocate memory for client_fd");
            continue;
        }
        *arg = client_fd;
        const int err = pthread_create(&client_thread, NULL, handleClient, arg);
        if (err) {
            printf("An error occurred: %d\n", err);
            continue;
        }
    }

    return 0;
}

void *threadHandleClient(void *arg) {
    int client_fd = *(int *) arg;
    free(arg);
    handleClient(client_fd);
    return NULL;
}

void handleClient(const int client_fd) {
}

// TODO: Remember to disconnect the client sockets before exiting the program
void handleExit() {
    exit(0);
}

void handleHelp() {
    printf("Available commands:\n");
    printf("  /help - show this message\n");
    printf("  /quit - exit the program\n");
}

void ask_for_input() {
    while (1) {
        char input[256];
        fgets(input, 255, stdin);

        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        if (strcmp(input, "/quit") == 0) {
            handleExit();
        } else if (strcmp(input, "/help") == 0) {
            handleHelp();
        } else {
            printf("Unknown command: %s\n", input);
        }
    }
}
