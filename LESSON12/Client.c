#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc,char** argv){
    int fd;
    char *dest;
    struct hostent *addr;
    struct sockaddr_in dstAddr;
    unsigned short serverPort;
    char buf[2048];

    if(argc != 3){
        fprintf(stderr,"USAGE:%s svr_addr \n",argv[0]);
        exit(-1);
    }
    dest = argv[1]; //set dest
    serverPort = atoi(argv[2]);

    memset((char*)&dstAddr,'\0',sizeof(dstAddr));
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(serverPort);

    //引数で指定したホスト名から、構造体hostentを返す
    addr = gethostbyname(dest);

    dstAddr.sin_addr.s_addr =* (unsigned int*) addr->h_addr_list[0];
    printf("%s \n",inet_ntoa(dstAddr.sin_addr)); //show IP addr

    if(addr == NULL){
        printf("no host\n");
        return -1;
    }

    if((fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        fprintf(stdout,"socket error\n");
        return -1;
    }

    //connect to server
    //sendするのにfdを使う。sendtoならいらないかも
    if(connect(fd,(struct sockaddr*)&dstAddr,sizeof(struct sockaddr_in))){
        perror("connect");
        exit(-1);
    }
    //送信リクエストメッセージの格納
    memcpy(buf,"This is the message sent by the client.",strlen("This is the message sent by the client."));

    if(send(fd,buf,strlen(buf),0) < 0){
        perror("send");
        close(fd);
        return -1;
    }

    memset(buf,'\0',sizeof(buf));
    if(recv(fd,buf,sizeof(buf),0) < 0){
        perror("recv");
        return -1;
    }
    printf("Received [%s] from Server.\n",buf);
    
    close(fd);
    return 0;
}