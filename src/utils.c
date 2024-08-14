#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

void printMessage(const struct Request *req) {
    printf("%s: %s\n", req->name, req->message);
}

void serialize(const struct Request *req, char *buf) {
    // copy "name" into buf
    memcpy(buf, req->name, BUFFER_SIZE);
    // copy "message" into buf
    memcpy(buf + BUFFER_SIZE, req->message, MESSAGE_SIZE);
}

void deserialize(const char *buf, struct Request *req) {
    // copy "name" from buf
    memcpy(req->name, buf, BUFFER_SIZE);
    // copy "message" from buf
    memcpy(req->message, buf + BUFFER_SIZE, MESSAGE_SIZE);
}

void sendRequest(const fd s, const struct Request *req) {
    char buf[sizeof(struct Request)];
    serialize(req, buf);

    if (send(s, buf, sizeof(buf), 0) < 0) {
        perror("send");
        close(s);
        exit(1);
    }
}

int recvRequest(const fd s, struct Request *req) {
    char buf[sizeof(struct Request)];

    int bytes_received;

    if ((bytes_received = recv(s, buf, sizeof(buf), 0)) < 0) {
        perror("recv");
        close(s);
        exit(1);
    }

    deserialize(buf, req);

    return bytes_received;
}
