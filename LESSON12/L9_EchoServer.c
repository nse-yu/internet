#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define MAXPENDING 5
#define RECVBUFSIZE 64

void HandleTCPClient(int);

int main(int argc, char *argv[]){
    int serverSock,clientSock;
    struct sockaddr_in echoServer_addr;
    struct sockaddr_in echoClient_addr;
    unsigned short echoServerPort;
    unsigned int clientLen;

    if(argc != 2){
        fprintf(stderr,"Usage: %s <Server Port>\n",argv[0]);
        exit(1);
    }

    //convert string to int
    echoServerPort = atoi(argv[1]);

    //create socket
    if((serverSock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0){
        perror("socket");
        exit(-1);
    }
    memset(&echoServer_addr,0,sizeof(echoServer_addr));
    
    //set some values
    //The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.
    echoServer_addr.sin_family = AF_INET;
    echoServer_addr.sin_port = htons(echoServerPort);
    echoServer_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket：serverで使用
    //bindはサーバー側で待ち受けIP・ポートを指定する
    if(bind(serverSock,(struct sockaddr*)&echoServer_addr,sizeof(echoServer_addr)) < 0){
        perror("bind");
        exit(-1);
    }

    //prepare to accept but return -1
    if(listen(serverSock,MAXPENDING) < 0){
        perror("listen");
    }

    while(1){
        clientLen = sizeof(echoClient_addr);
        //acceptでは、clientのsock情報を受け入れるために、sockaddrはclient用を指定。長さも
        if((clientSock = accept(serverSock,(struct sockaddr*)&echoClient_addr,&clientLen)) < 0){
            perror("accept");
        }

        printf("Handling client %s\n",inet_ntoa(echoClient_addr.sin_addr));
        HandleTCPClient(clientSock);
    }
}

void HandleTCPClient(int clientSock){
    /*clientから受け取るデータサイズ分のchar配列*/
    char echoBuffer[RECVBUFSIZE];
    int recvSize;

    if((recvSize = recv(clientSock,echoBuffer,RECVBUFSIZE,0)) < 0){
        perror("recv");
        exit(-1);
    }
    printf("Received [%s] from Client.\n",echoBuffer);

    //serverからclientに、client socket descの情報を元に送り返す。
    //正常送信ならば送ったmsgと同じバイト数が返される
    if(send(clientSock,echoBuffer,recvSize,0) < 0){
        perror("send");
    }

    close(clientSock);
}