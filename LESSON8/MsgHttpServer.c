#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int httpd(int);
int send_msg(int,char *);

int main(void){
    int sock,new_sock;
    struct sockaddr_in addr,new_addr;
    socklen_t len;
    char buf[200];

    //create empty socket
    sock = socket(AF_INET,SOCK_STREAM,0);

    //address field
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);

    //give socket a name
    if(bind(sock,(struct sockaddr*)&addr,sizeof(addr)) < 0){
        perror("bind");
        return 1;
    }

    //preparation
    if(listen(sock,5) < 0){
        perror("listen");
        return 1;
    }

    while(1){
        len = sizeof(new_addr); 

        //waiting
        if((new_sock = accept(sock,(struct sockaddr*)&new_addr,&len)) < 0){
            perror("accept");
            return 1;
        }

        //proceed to the http process
        httpd(new_sock);
        close(new_sock);      
    }
    close(sock);

    return 0;
}

//method to perform the http process
int httpd(int recv_sock){
    char buf[256];
    char method[16],uri[128],httpVer[32];
    char *fp_uri;
    char line[128];
    char msg[256];

    //get messages into buffer from the received socket
    if(recv(recv_sock,buf,sizeof(buf),0) < 0){
        perror("recv");
        return 1;
    }
    //read formatted input string from buf
    sscanf(buf,"%s %s %s",method,uri,httpVer);
    
    if(strcmp(method,"GET") > 0){
        send_msg(recv_sock,"501 Not Implemented");
    }

    //response header
    send_msg(recv_sock,"HTTP/1.0 200 OK\r\n");
    send_msg(recv_sock,"text/html\r\n");
    send_msg(recv_sock,"\r\n");

    printf("文字列を入力：");
    while(fgets(line,100,stdin) != NULL){  
        printf("もう一度\n");      
        sprintf(msg,"<html><body><h1>%s</h1></body></html>",line);

        if(send(recv_sock,msg,strlen(msg),0) < 0){
            perror("send");
            return 1;
        }
    }
    printf("fin");

    close(recv_sock);
}

//returns length of messages which could have sent  
int send_msg(int conn_sock,char *msg){
    int len = strlen(msg);

    if(send(conn_sock,msg,len,0) < 0){
        perror("send");
        return 1;
    }
    return len;
}