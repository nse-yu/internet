#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    int sock,acsock;
    int rsize;
    struct sockaddr_in addr,client;
    int len;
    int yes = 1;
    char buf[4096];
    char inbuf[4096];
    char strhead[100];
    char main[100] = "<html><body><h1>HELLO World!!</h1></body></html>\r\n";
    
    if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket");
        return 1;
    }

    //insert values to address field
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = INADDR_ANY;

    //sock establishment as a receiver
    if(bind(sock,(struct sockaddr *)&addr,sizeof(addr)) < 0){
        perror("bind");
        return 1;
    }

    if(listen(sock,5) != 0){
        perror("listen");
        return 1;
    }
    
    while(1){
        len = sizeof(client);
        //acceptは、リクエスト処理用の新しいsocketを返す
        if((acsock = accept(sock,(struct sockaddr*)&client,&len)) < 0){
            perror("accept");
            break;
        }
        //Insert terminated characters to area pointed to by the buffer
        memset(buf,'\0',sizeof(buf));

        snprintf(buf,sizeof(buf),
            "HTTP/1.0 200 OK\r\n"
            "Content-Length: 0000\r\n"
            "Content-Type: text/html\r\n""\r\n"
            "<html><body><h1>HELLO World!!</h1></body></html>\r\n"
        );

        memset(inbuf,'\0',sizeof(inbuf));
        //socketからデータを読み取る
        if((rsize = recv(acsock,inbuf,sizeof(inbuf),0)) < 0){
            perror("recv");
            return -1;
        }

        sprintf(strhead,
            "HTTP/1.0 200 OK\r\n"
            "Content-Length: %ld\r\n"
            "Content-Type: text/html\r\n""\r\n",
            strlen(main)+strlen(inbuf));
        strcpy(buf,strcat(strhead,main));
        //現在のbufに続いて、受け取ったinbufをそのサイズ分だけbuf領域に格納
        memcpy(buf+strlen(buf),inbuf,rsize);
        printf("%s",buf);

        if(send(acsock,buf,strlen(buf),0) < 0){
            perror("send");
            return -1;
        }
        close(acsock);
    }
    close(sock);
    return 0;
}