#ifndef UTILS_H
#define UTILS_H
#include <common.h>

void printMessage(const struct Request *req);

void serialize(const struct Request *req, char *buf);

void deserialize(const char *buf, struct Request *req);

void sendRequest(const fd s, const struct Request *req);

int recvRequest(const fd s, struct Request *req);

#endif //UTILS_H
