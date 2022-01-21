#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define MAXPENDING 5
#define RECVBUFSIZE 32

/**エラーをcatchしたときに、エラー表示したい文字列とともに呼び出す
 * 異常終了としてシステムに送信
*/
void erroHandler(char *msg){
    perror(msg);
    exit(1);
}

void HandleTCPClient(int clientSock){
    /**/
    char echoBuffer[RECVBUFSIZE];
    /**/
    int recvMsgSize;

    //recvはエラー時に-1を返すため、エラー処理をする
    if((recvMsgSize = recv(clientSock,echoBuffer,RECVBUFSIZE,0)) < 0){
        erroHandler("recv() failed");
    }

    while(recvMsgSize > 0){
        //serverからclientに、client socket descの情報を元に送り返す。
        //正常送信ならば送ったmsgと同じバイト数が返される
        if(send(clientSock,echoBuffer,recvMsgSize,0) != recvMsgSize){
            erroHandler("send() failed");
        }

        if((recvMsgSize = recv(clientSock,echoBuffer,RECVBUFSIZE,0) < 0)){
            erroHandler("recv() failed");
        }
    }
    close(clientSock);
}