#ifndef COMMON_H
#define COMMON_H

#define PORT 8080
#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 256
#define MESSAGE_SIZE BUFFER_SIZE * 2
#define DEFAULT_INIT_CONNECTION_MESSAGE "init"

typedef int fd;

#define TYPE_MESSAGE 0
#define TYPE_JOIN 1

struct Request {
    char name[BUFFER_SIZE];
    char message[MESSAGE_SIZE];
};

#endif //COMMON_H
