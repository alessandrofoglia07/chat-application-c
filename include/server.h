#ifndef SERVER_H
#define SERVER_H

void *askForInput(void *arg);

void handleExit();

void handleHelp();

void *threadHandleClient(void *arg);

void handleClient(const fd client_fd);

void broadcastMessage(const char *message);

#endif //SERVER_H
