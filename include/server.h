#ifndef SERVER_H
#define SERVER_H

#define PORT 8080
#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 256

typedef int fd;

void askForInput();

void handleExit();

void handleHelp();

void *threadHandleClient(void *arg);

void handleClient();

void broadcastMessage(const char *message);

#endif //SERVER_H
