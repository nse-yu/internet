#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in s_addr,c_addr;
    socklen_t c_addrlen;
    char buf[2048],c_port[10];
    char compare[10] = "request";

    /*UDPでは、データ送信のための接続確立を行わない代わりに、
    Server側ではそれを待ち受けするためにsocketをbind（名前付け）
    する必要がある。*/

    //Clientを受け入れるためのsocketを生成
    sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0){
        perror("sock");
        return 1;
    }
    //server側のIPとポートを構造体変数に格納
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(atoi(argv[1]));
    s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //socketを確立させる
    if(bind(sock,(struct sockaddr *)&s_addr,sizeof(s_addr)) < 0){
        perror("bind");
        return 1;
    }

    while(1){
        
        //client情報受け入れ準備
        memset(buf,0,sizeof(buf));
        c_addrlen = sizeof(c_addr);

        //clientからのリクエストを受け取る：リクエストが来るまでは待受
        //client_addrにクライアントのIP・ポート情報が入るため、sendtoするときには
        //この情報（client_addr）を利用すればよい
        if(recvfrom(sock,buf,sizeof(buf),0,
        (struct sockaddr*)&c_addr,&c_addrlen) < 0){
            perror("recv");
            return 1;
        }

        //bufから「:」を検索し、その位置のポインタを得る
        char* p_buf = memchr(buf,':',strlen(buf));
        memcpy(c_port,p_buf+1,5);
        c_port[5] = '\0';

        //リクエストを受け取った後に送信
        char message[20] = "Hello_World";

        //port格納
        c_addr.sin_port = htons(atoi(c_port));
        if(sendto(sock,message,sizeof(message),0,(struct sockaddr*)&c_addr,sizeof(c_addr)) < 0){
            perror("sendto");
            return 1;
        }

        //結果表示
        printf("buf = %s\n",buf);
        printf("client indicated the port: %s\n",c_port);
        printf("received from port : %d\n",htons(c_addr.sin_port));
    }
    //close the socket
    close(sock);

    return 0;
}