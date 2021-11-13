#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
//Client
int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in server_addr,senderInfo;
    socklen_t addrlen;
    unsigned short echoClientPort;
    char buf[2048];

    //get the port number from user's assignment
    echoClientPort = atoi(argv[1]);
    //construct a socket about server : SOCK_DGRAM uses UDP protocol
    sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0){
        perror("sock");
        return 1;
    }

    //server(sender) address field's values setting
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(echoClientPort);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //bind socket to server using some informations(server_addr..)
    if(bind(sock,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
        perror("bind");
        return 1;
    }

    //create the buffer area in memory
    memset(buf,0,sizeof(buf));
    addrlen = sizeof(senderInfo);

    //receive information into buf through sock and senderInfo will be filled about addr
    if(recvfrom(sock,buf,sizeof(buf)-1,0,
    (struct sockaddr*)&senderInfo,&addrlen) < 0){
        perror("recv");
        return 1;
    }

    //print a received character
    printf("%s\n",buf);

    //close the socket
    close(sock);

    return 0;
}