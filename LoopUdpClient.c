#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
    int s_sock,c_sock;
    struct sockaddr_in s_addr,c_addr;
    socklen_t c_addrlen;
    unsigned int s_port,c_port;
    char buf[200],str[100];

    //convert ASCII to Integer
    s_port = atoi(argv[1]); //arg1 : server port
    c_port = atoi(argv[2]); //arg2 : client port

    //create the socket
    s_sock = socket(AF_INET,SOCK_DGRAM,0);
    c_sock = socket(AF_INET,SOCK_DGRAM,0);
    if(s_sock < 0 || c_sock < 0){
        perror("socket");
        return 1;
    }

    //1.送信するサーバーのIPとポートを定義
    //2.サーバーから受け取るためのIPとポートを定義
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(s_port);
    s_addr.sin_addr.s_addr = INADDR_ANY;

    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(c_port);
    c_addr.sin_addr.s_addr = INADDR_ANY;

    //client用socketの確立
    if(bind(s_sock,(struct sockaddr*)&c_addr,sizeof(c_addr)) < 0){
        perror("bind");
        return 1;
    }
    
    //リクエストメッセージを格納
    sprintf(str,"request:%d",c_port);
    //リクエストの送信：s_sockは書き込まれる：構造体は送信先を指定
    if(sendto(s_sock,str,sizeof(str),0,(struct sockaddr*)&s_addr,sizeof(s_addr)) < 0){
        perror("sendto");
        return 1;
    }

    c_addrlen = sizeof(c_addr);
    //サーバーからのデータ受け取り：s_sockは読み込まれる：構造体は受取側のアドレス
    if(recvfrom(s_sock,buf,sizeof(buf),0,(struct sockaddr*)&c_addr,&c_addrlen) < 0){
        perror("recvfrom");
        return 1;
    }

    //結果表示    
    printf("received message : %s\n",buf);
    printf("send request = %s\n",str);
    printf("received from port : %d\n",ntohs(c_addr.sin_port));

    close(s_sock);
    close(c_sock);

    return 0;
}