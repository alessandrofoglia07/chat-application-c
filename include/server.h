#ifndef SERVER_H
#define SERVER_H
#include <common.h>

void *askForInput(void *arg);

void handleExit();

void handleHelp();

void *threadHandleClient(void *arg);

void handleClient(const fd client_fd);

void broadcastMessage(const char *message)

void broadcastMessageExcludeSender(const char *message, const fd client_fd);

#endif //SERVER_H
