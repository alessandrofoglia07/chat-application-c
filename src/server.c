#include "server.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

int activeClientsCount = 0;
fd activeClients[MAX_CONNECTIONS];
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    const fd s = socket(AF_INET, SOCK_STREAM, 0);

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
    const int serverInputThreadError = pthread_create(&serverInputThread, NULL, askForInput, NULL);
    if (serverInputThreadError) {
        printf("An error occurred: %d\n", serverInputThreadError);
        return 1;
    }

    while (1) {
        const fd client_fd = accept(s, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        pthread_t client_thread;

        fd *arg = malloc(sizeof(client_fd));
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

void removeClient(const fd client_fd) {
    int index = -1;
    for (int i = 0; i < activeClientsCount; i++) {
        if (activeClients[i] == client_fd) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        for (int i = index; i < activeClientsCount - 1; i++) {
            activeClients[i] = activeClients[i - 1];
        }
        activeClientsCount--;
    }
}

void broadcastMessage(const char *message) {
    for (int i = 0; i < activeClientsCount; i++) {
        send(activeClients[i], message, sizeof(message), 0);
    }
}

void *threadHandleClient(void *arg) {
    int client_fd = *(fd *) arg;
    free(arg);
    handleClient(client_fd);
    return NULL;
}

void handleClient(const fd client_fd) {
    pthread_mutex_lock(&clientsMutex);
    activeClients[activeClientsCount++] = client_fd;
    pthread_mutex_unlock(&clientsMutex);

    // another scope to avoid using the same variable name
    char message[256];
    snprintf(message, 255, "Client %d has joined the chat\n", client_fd);
    broadcastMessage(message);

    char buf[BUFFER_SIZE];
    while (1) {
        const int bytes_read = recv(client_fd, buf, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                // client disconnected
                printf("Client %d has disconnected\n", client_fd);
            } else {
                perror("recv");
            }
            break;
        }

        buf[bytes_read] = '\0';
        printf("Client %d: %s\n", client_fd, buf);
        broadcastMessage(buf);
    }

    pthread_mutex_lock(&clientsMutex);
    // remove the client from the list
    removeClient(client_fd);
    pthread_mutex_unlock(&clientsMutex);

    // notify other clients that this client has left
    snprintf(message, 255, "Client %d has left the chat\n", client_fd);
    broadcastMessage(message);

    close(client_fd);
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

void askForInput() {
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
