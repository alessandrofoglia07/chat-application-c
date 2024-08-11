#ifndef SERVER_H
#define SERVER_H
#include <common.h>

struct Client {
    char name[BUFFER_SIZE];
    fd client_fd;
};

void *askForInput(void *arg);

void handleExit();

void handleHelp();

void *threadHandleClient(void *arg);

void handleClient(const fd client_fd);

void broadcastMessage(const struct Request *req);

void broadcastMessageExcludeSender(const struct Request *req, const fd client_fd);

#endif //SERVER_H
