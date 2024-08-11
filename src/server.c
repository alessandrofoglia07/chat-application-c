#include "server.h"
#include <common.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

int activeClientsCount = 0;
struct Client activeClients[MAX_CONNECTIONS];
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    signal(SIGINT, handleExit); // gracefully handle Ctrl+C
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

    // Set socket options
    const int optval = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Setsockopt failed");
        close(s);
        exit(EXIT_FAILURE);
    }

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

    printf("Server listening at port %d\n", PORT);

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

        if (activeClientsCount >= MAX_CONNECTIONS) {
            printf("Maximum number of clients reached\n");
            close(client_fd);
            continue;
        }

        pthread_t client_thread;

        fd *arg = malloc(sizeof(client_fd));
        if (arg == NULL) {
            perror("Failed to allocate memory for client_fd");
            continue;
        }
        *arg = client_fd;
        const int err = pthread_create(&client_thread, NULL, threadHandleClient, arg);
        if (err) {
            printf("An error occurred: %d\n", err);
            free(arg);
        }
    }
}

void removeClient(const fd client_fd) {
    int index = -1;
    for (int i = 0; i < activeClientsCount; i++) {
        if (activeClients[i].client_fd == client_fd) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        for (int i = index; i < activeClientsCount - 1; i++) {
            activeClients[i] = activeClients[i + 1];
        }
        activeClientsCount--;
    }
}

void broadcastMessage(const struct Request *req) {
    for (int i = 0; i < activeClientsCount; i++) {
        sendRequest(activeClients[i].client_fd, req);
    }
}

void broadcastMessageExcludeSender(const struct Request *req, const fd client_fd) {
    for (int i = 0; i < activeClientsCount; i++) {
        if (activeClients[i].client_fd == client_fd) {
            continue;
        }
        sendRequest(activeClients[i].client_fd, req);
    }
}

void *threadHandleClient(void *arg) {
    const int client_fd = *(fd *) arg;
    free(arg);
    handleClient(client_fd);
    return NULL;
}

void handleClient(const fd client_fd) {
    struct Request req;

    // first message is for the client to set their name
    recvRequest(client_fd, &req);

    // check if name is already taken
    int clientExists = 0;
    for (int i = 0; i < activeClientsCount; i++) {
        if (req.name == activeClients[i].name) {
            clientExists = 1;
            break;
        }
    }

    // if client exists or the message is not "init", send a message to the client and return
    if (clientExists || strcmp(req.message, "init") != 0) {
        snprintf(req.message, MESSAGE_SIZE, "Server: Client %s already exists. Please choose another name",
                 req.name);
        strcpy(req.name, "Server");
        sendRequest(client_fd, &req);
        close(client_fd);
        return;
    }

    struct Client client;
    strcpy(client.name, req.name);
    client.client_fd = client_fd;

    pthread_mutex_lock(&clientsMutex);
    activeClients[activeClientsCount++] = client;
    pthread_mutex_unlock(&clientsMutex);

    // if initial request is good, broadcast that the client has joined
    strcpy(req.name, "Server");
    snprintf(req.message, MESSAGE_SIZE, "Client %s has joined the chat", req.name);
    printf("%s\n", req.message);
    broadcastMessageExcludeSender(&req, client_fd);

    while (1) {
        const int bytes_read = recvRequest(client_fd, &req);
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                // client disconnected
                printf("Client %d has left the chat.\n", client_fd);
            } else {
                perror("recv");
            }
            break;
        }

        printf("%s\n", req.message);
        broadcastMessageExcludeSender(&req, client_fd);
    }

    pthread_mutex_lock(&clientsMutex);
    // remove the client from the list
    removeClient(client_fd);
    pthread_mutex_unlock(&clientsMutex);

    // notify other clients that this client has left
    strcpy(req.name, "Server");
    snprintf(req.message, MESSAGE_SIZE, "Server: Client %s has left the chat", client.name);
    broadcastMessageExcludeSender(&req, client_fd);

    close(client_fd);
}

void handleExit() {
    const struct Request req = {
        .name = "Server",
        .message = "The server is shutting down"
    };
    broadcastMessage(&req);
    exit(0);
}

void handleHelp() {
    printf("Available commands:\n");
    printf("  /help - show this message\n");
    printf("  /quit - exit the program\n");
}

void *askForInput(void *arg) {
    while (1) {
        char input[256];
        fgets(input, 255, stdin);

        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        if (strcmp(input, "/send") == 0) {
            struct Request req;
            strcpy(req.name, "Server");
            printf("Enter your message: ");
            fgets(req.message, MESSAGE_SIZE, stdin);
            broadcastMessage(&req);
        } else if (strcmp(input, "/quit") == 0) {
            handleExit();
        } else if (strcmp(input, "/help") == 0) {
            handleHelp();
        } else if (input[0] == '/') {
            printf("Unknown command: %s\n", input);
        }
    }
}
