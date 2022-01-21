#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
    int wport;
    int sock;
    struct sockaddr_in snder,rcv;
    socklen_t snderlen;
    char buf[256];
    socklen_t addrlen;
    int rsize;

    if(argc != 5){
        printf("Usage:%s rcv-port snd-addr snd-port msg \n",argv[0]);
        exit(-1);
    }

    sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0){
        perror("socket");
        return 1;
    }

    rcv.sin_family = AF_INET;
    rcv.sin_port = htons(atoi(argv[1]));
    rcv.sin_addr.s_addr = INADDR_ANY;

    snder.sin_family = AF_INET;
    snder.sin_port = htons(atoi(argv[3]));
    snder.sin_addr.s_addr = inet_addr(argv[2]);

    if(bind(sock,(struct sockaddr *)&rcv,sizeof(rcv)) < 0){
        perror("bind");
        return 1;
    }

    //send requests
    /*接続先のソケット型がSOCK_STREAM, SOCK_SEQPACKETでなければ、sockaddrのアドレスから送信先を指定*/
    //sock -> 自分のポート
    if(sendto(sock,argv[4],strlen(argv[4]),0,(struct sockaddr *)&snder,sizeof(snder)) < 0){
        perror("send");
        return 1;
    }

    snderlen = sizeof(snder);
    memset(buf,'\0',sizeof(buf));

    if((rsize = recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr *)&snder,&snderlen)) < 0){
        perror("recv");
        return 1;
    } 
    printf("received from port %d \n",ntohs(snder.sin_port));
    printf("%s\n",buf);
    printf("size: %d \n",rsize);
    
    close(sock);

    return 0;
}