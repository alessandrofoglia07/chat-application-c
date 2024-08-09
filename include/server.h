#ifndef SERVER_H
#define SERVER_H

#define PORT 8080
#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 256

void ask_for_input();

void handleExit();

void handleHelp();

void *threadHandleClient(void *arg);

void handleClient();

#endif //SERVER_H
