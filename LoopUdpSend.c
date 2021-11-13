#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
//Server
int main(int argc,char *argv[]){
    int sock,reqest_len = sizeof("request");
    struct sockaddr_in addr,receiverInfo;
    socklen_t addlen;
    unsigned short port;
    char buf[2048];

    //convert ASCII to Integer
    port = atoi(argv[1]);
    //create the socket
    sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0){
        perror("socket");
        return 1;
    }

    //initialize the fields of sockaddr_in
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //create the buffer area in memory
    memset(buf,0,sizeof(buf));
    addlen = sizeof(receiverInfo);

    if(recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&receiverInfo,&addlen) < 0){
        perror("recvfrom(send)");
        return 1;
    }
    
    if(memcmp(buf,"request",reqest_len) < 0){
        perror("memcmp");
    }else{
        /*ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen);*/
    //SOCK_STREAM, SOCK_SEQPACKETを用いた通信ならばsend()と同じになるようだ
    if(sendto(sock,"Hello_World",11,0,(struct sockaddr*)&addr,sizeof(addr)) < 0){
        perror("send");
        return 1;
    }
    }
    close(sock);

    return 0;
}