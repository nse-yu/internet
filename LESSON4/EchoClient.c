#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "header/handler.h"

int main(int argc,char *argv[]){
    int desc;
    struct sockaddr_in server;
    char buf[256];
    int n;
    char echoString[32];
    unsigned short echoClientPort;
    unsigned int echoStringlen = strlen(echoString);

    if(argc != 3){
        fprintf(stderr,"Usage: %s <Client Port>\n",argv[0]);
        exit(1);
    }
    echoClientPort = atoi(argv[1]);
    strcpy(echoString,argv[2]);

    //create socket but return -1
    if((desc = socket(AF_INET,SOCK_STREAM,0)) < 0){
        erroHandler("socket() failded");
    }
    //set some values to struct sockaddr_in about server
    server.sin_family = AF_INET;
    server.sin_port = htons(echoClientPort);
    //127.0.0.1は自分自身を指すアドレス
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    //connect to server ：client->serverへの接続に使用
    /*fdをaddrに結び付ける*/
    /*ストリーム・ソケットの場合、connect() 呼び出し は 2 つのソケット間の接続を確立しようとします。
    データグラム・ソケットの場合、connect() 呼び出しはソケットのピアを指定します。
    socket パラメーターは、接続要求の発信に 使用されるソケットです。
    connect() 呼び出しは、ストリーム・ソケット用に 呼び出されたときに、2 つのタスクを実行します。
    まず、この呼び出しはストリーム・ソケットに必要な バインディングを完了します (bind() 呼び出しを 使用して事前にバインディングが完了していない場合)。
    次に、この呼び出しは別のソケットへの接続を試行します。*/
    if((connect(desc,(struct sockaddr*)&server,sizeof(server))) < 0){
        erroHandler("connect() failed");
    }

    //send to server but can't send all String
    if((send(desc,echoString,echoStringlen,0)) < 0){
        erroHandler("send() failed");
    }

    //receive data from server
    /*第一引数の先頭ポインタから第三引数バイトまでのメモリエリアを第二引数で埋める*/
    //memsetはおそらく、領域確保のためにおこなう
    memset(buf,0,sizeof(buf));

    //第一引数から第三引数バイトを第二引数に読み込み、読み込んだバイト数を返す
    /*recv() 関数は、記述子 socket を用いてソケット上のデータを受信し、バッファーに保管します。
    recv() 呼び出しは、接続されたソケットだけに適用されます。*/
    if((n = recv(desc,buf,sizeof(buf),0)) < 0){
        erroHandler("recv() failed");
    }
    printf("%d, %s\n",n,buf);

    //close socket
    close(desc);

    return 0;
}