#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
    int sock,client_sock;
    struct sockaddr_in my_addr,client_addr;
    socklen_t C_addrlen;
    unsigned short myPort;
    char buf[2048];
    char compare[10] = "request";

    /*UDPでは、データ送信のための接続確立を行わない代わりに、
    Server側ではそれを待ち受けするためにsocketをbind（名前付け）
    する必要がある。*/

    //待ち受けポート番号を指定
    myPort = atoi(argv[1]);
    //Clientを受け入れるためのsocketを生成
    sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0){
        perror("sock");
        return 1;
    }
    //server側のIPとポートを構造体変数に格納
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(myPort);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //socketを確立させる
    if(bind(sock,(struct sockaddr *)&my_addr,sizeof(my_addr)) < 0){
        perror("bind");
        return 1;
    }

    //client情報受け入れ準備
    memset(buf,0,sizeof(buf));
    C_addrlen = sizeof(client_addr);

    //socketからのリクエストを受け取る：リクエストが来るまでは待受
    //client_addrにクライアントのIP・ポート情報が入るため、sendtoするときには
    //この情報（client_addr）を利用すればよい
    if(recvfrom(sock,buf,sizeof(buf)-1,0,
    (struct sockaddr*)&client_addr,&C_addrlen) < 0){
        perror("recv");
        return 1;
    }
    if(memcmp(buf,compare,strlen(compare)) != 0){
        printf("not succeeded");
    }else{
        //リクエストを受け取った後に送信
        char message[20] = "Hello_World";
        if(sendto(sock,message,sizeof(message),0,(struct sockaddr*)&client_addr,sizeof(client_addr)) < 0){
            perror("send");
            return 1;
        }
    }
    //close the socket
    close(sock);

    return 0;
}