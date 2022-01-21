#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
    int wport;
    int rsock,ssock;
    struct sockaddr_in rcver,snder;
    char buf[256];
    socklen_t addrlen;
    int rsize;

    if(argc != 2){
        printf("Usage:%s rcv-port \n",argv[0]);
        exit(-1);
    }

    //convert ascii to integer
    wport = atoi(argv[1]);

    //create sockets
    rsock = socket(AF_INET,SOCK_DGRAM,0);
    ssock = socket(AF_INET,SOCK_DGRAM,0);

    if(rsock < 0 || ssock < 0){
        perror("socket ");
        return -1;
    }

    //insert values to address field
    rcver.sin_family = AF_INET;
    rcver.sin_port = htons(wport);
    rcver.sin_addr.s_addr = INADDR_ANY;

    //sock establishment as a receiver
    if(bind(rsock,(struct sockaddr *)&rcver,sizeof(rcver)) < 0){
        perror("bind");
        return 1;
    }

    //start message for loop
    printf("start \n");

    while(1){
        //Insert terminated characters to area pointed to by the buffer
        memset(buf,'\0',sizeof(buf));
        addrlen = sizeof(snder);

        if((rsize = recvfrom(rsock,buf,sizeof(buf)-1, 0, (struct sockaddr *)&snder,&addrlen)) < 0){
            perror("recv");
            return 1;
        }
        //display messages in the buffer
        printf("%s \n",buf);
        //display information in the addr field
        printf("received size %d from %s , port %d \n",
        rsize,inet_ntoa(snder.sin_addr),ntohs(snder.sin_port));

        //snderがrecvfromで相手情報に初期化されているから返送ができる。
        //ここのrsockがssockになってもなぜか適切な情報が送れる。なぜかわからん。
        if(sendto(rsock,buf,rsize,0,(struct sockaddr *)&snder,addrlen) < 0){
            perror("send");
            return 1;
        }
    }
    close(rsock);
    close(ssock);

    return 0;
}