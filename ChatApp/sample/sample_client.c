#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 64

int main(int argc,char** argv){
    int sock;
    struct sockaddr_in s_addr;
    char buf[BUF_SIZE];

    //initialize the buf
    memset(buf,'\0',sizeof(buf));

    //create the socket
    if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0){
        fprintf(stderr,"socket() err\n");
        exit(EXIT_FAILURE);
    }

    //set values
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(atoi(argv[1]));
    s_addr.sin_addr.s_addr = INADDR_ANY;

    //open a connection to server addr
    if(connect(sock,(struct sockaddr*)&s_addr,sizeof(s_addr)) < 0){
        fprintf(stderr,"connect() err\n");
        exit(EXIT_FAILURE);
    }

    //set a message
    memcpy(buf,"UNKO",strlen("UNKO"));

    //sleep
    sleep(3);

    //send messages to server
    if(send(sock,buf,sizeof(buf),0) < 0){
        fprintf(stderr,"send() err\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("[send] %s\n",buf);

    //reinitialize the buf
    memset(buf,'\0',sizeof(buf));

    //after sending, receive messages from server
    if(recv(sock,buf,sizeof(buf),0) < 0){
        fprintf(stderr,"recv() err\n");
        close(sock);
        exit(EXIT_FAILURE);        
    }
    printf("[recv] %s\n",buf);

    //close the socket
    close(sock);

    return 0;
}