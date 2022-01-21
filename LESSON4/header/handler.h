#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define MAXPENDING 5
#define RECVBUFSIZE 32

void erroHandler(char *msg);
void HandleTCPClient(int clientSock);