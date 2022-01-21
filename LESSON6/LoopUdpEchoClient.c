#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
//[自分ポート、相手IP、相手ポート、送りたい文字列]の順にコマンドライン引数指定
int main(int argc,char *argv[]){
    int wport;
    int sock;
    struct sockaddr_in snder,rcv;
    socklen_t snderlen;
    char buf[256],message[100];
    socklen_t addrlen;
    int rsize;

    if(argc != 4){
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

    //自分のアドレス・ポートを格納
    //送信するsockがbindできてないと、相手に正しい情報を引き渡すことができない。
    //（bindしないと、ポートが変わったりとかが起きる）
    if(bind(sock,(struct sockaddr *)&rcv,sizeof(rcv)) < 0){
        perror("bind");
        return 1;
    }

    do{
        printf("送信メッセージ：");
        scanf("%s",message);

        //send requests, sockにはこちら側の情報が入っているため、向こう側で誰が送ってきたのかを確認できる。
        if(sendto(sock,message,strlen(message),0,(struct sockaddr *)&snder,sizeof(snder)) < 0){
            perror("send");
            return 1;
        }

        snderlen = sizeof(snder);
        //buf領域をすべて終端文字列で埋める
        memset(buf,'\0',sizeof(buf));

        //最後の終端文字列以外を受け取った文字列で埋める, 多分ここのsockはなんでもいい
        if((rsize = recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr *)&snder,&snderlen)) < 0){
            perror("recv");
            return 1;
        } 
        printf("received from port %d \n",ntohs(snder.sin_port));
        printf("%s\n",buf);
        printf("size: %d \n",rsize);
    }while(strcmp(message,"exit") != 0);
    
    close(sock);

    return 0;
}