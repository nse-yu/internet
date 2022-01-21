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

int main(int argc, char *argv[]){
    int sv_sock,cl_sock;
    struct sockaddr_in addr,cl_addr;
    int size,len;
    char buf[32];

    if(argc != 2){
        printf("please input a port number\n");
        return 1;
    }

    sv_sock = socket(AF_INET,SOCK_STREAM,0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(atoi(argv[1]));

    if(bind(sv_sock,(struct sockaddr *)&addr,sizeof(addr)) < 0){
        perror("bind");
        return 1;
    }

    if(listen(sv_sock,3) < 0){
        perror("listen");
        return 1;
    }
    
    len = sizeof(cl_addr);
    if((cl_sock = accept(sv_sock,(struct sockaddr *)&cl_addr,&len)) < 0){
        perror("cl_sock");
        return 1;
    }
    sleep(5);
    sprintf(buf,"hello");
    if((size = send(cl_sock,buf,sizeof(buf),0)) < 0){
        perror("send");
        printf("failed!! sent byte : %d",size);
    }
    close(sv_sock);
    close(cl_sock);

    return 0;
}