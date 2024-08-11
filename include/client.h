#ifndef CLIENT_H
#define CLIENT_H

#define SERVER_ADDR "127.0.0.1"
#include <common.h>

extern char name[BUFFER_SIZE];

void *sendMessages(void *socketDesc);

void *recvMessages(void *socketDesc);

#endif //CLIENT_H
