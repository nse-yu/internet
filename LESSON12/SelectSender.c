#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//複数のホストにデータを送信する一番簡単な実装：複数のアドレスをstruct定義してそこにそれぞれ送信
int main(){
    int sock;
    struct sockaddr_in dest1,dest2;
    char buf[2048];

    sock = socket(AF_INET,SOCK_DGRAM,0);

    //２つのアドレスを定義
    dest1.sin_family = AF_INET;
    dest2.sin_family = AF_INET;
    dest1.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    dest2.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest1.sin_port = htons(11111);
    dest2.sin_port = htons(22222);

    //bufに格納された文字列を、socketでdest1宛てに送信
    memset(buf,'\0',sizeof(buf));
    memcpy(buf,"data to port 11111",strlen("data to port 11111"));
    sendto(sock,buf,strlen(buf),0,(struct sockaddr*)&dest1,sizeof(dest1)); 

    //bufに格納された文字列を、socketでdest2宛てに送信
    memset(buf,'\0',sizeof(buf));
    memcpy(buf,"data to port 22222",strlen("data to port 22222"));
    sendto(sock,buf,strlen(buf),0,(struct sockaddr*)&dest2,sizeof(dest2)); 

    close(sock);

    return 0;
}