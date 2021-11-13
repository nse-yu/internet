#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
int main(int argc,char *argv[]){
    int sock;
    struct sockaddr_in server_addr;
    socklen_t S_addrlen;
    unsigned int port;
    char buf[200],str[100];

    //convert ASCII to Integer
    port = atoi(argv[1]);
    //create the socket
    sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0){
        perror("socket");
        return 1;
    }
    //initialize the fields of sockaddr_in
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    //リクエストメッセージを格納
    sprintf(str,"request:%d",port);
    //リクエストの送信
    if(sendto(sock,str,sizeof(str),0,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
        perror("sendto");
        return 1;
    }

    S_addrlen = sizeof(server_addr);
    if(recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr*)&server_addr,&S_addrlen) < 0){
        perror("recvfrom");
        return 1;
    }
    printf("received message : %s\n",buf);
    printf("send request = %s\n",str);

    close(sock);

    return 0;
}