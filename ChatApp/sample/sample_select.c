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
    int sock,new_sock;
    struct sockaddr_in addr;
    char buf[BUF_SIZE],new_buf[BUF_SIZE];
    int addr_size;
    fd_set read;

    //initialize
    memset(buf,'\0',sizeof(buf));
    FD_ZERO(&read);

    //create the socket
    if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0){
        fprintf(stderr,"socket() err\n");
        exit(EXIT_FAILURE);
    }

    //set values
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr_size = sizeof(addr);

    //establishing the socket
    if(bind(sock,(struct sockaddr*)&addr,sizeof(addr)) < 0){
        fprintf(stderr,"bind() err\n");
        exit(EXIT_FAILURE);
    }

    //prepare to accept
    listen(sock,3);

    //wait and connect
    if((new_sock = accept(sock,(struct sockaddr*)&addr,&addr_size)) < 0){
        fprintf(stderr,"accept() err\n");
        exit(EXIT_FAILURE);
    }
    printf("[accept] id: %d\n",new_sock);

    //monitor the new socket for reading
    FD_SET(new_sock,&read);
    printf("[set] id: %d\n",new_sock);

    while(1){
        int num_of_ready_desc = select(new_sock+1,&read,NULL,NULL,NULL);
        printf("[ready] count: %d\n",num_of_ready_desc);
        fflush(stdout);

        //ready to read
        if(FD_ISSET(new_sock,&read)){
            //receive messages from client
            if(recv(new_sock,buf,sizeof(buf),0) < 0){
                fprintf(stderr,"recv() err\n");
                close(new_sock);
            }
            printf("[recv] %s\n",buf);
            break;
        }
    }

    //send messages to client
    sprintf(new_buf,"You're %s !!",buf);
    if(send(new_sock,new_buf,sizeof(new_buf),0) < 0){
        fprintf(stderr,"send() err\n");
        close(new_sock);
        exit(EXIT_FAILURE);
    }

    return 0;
}