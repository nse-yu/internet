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
#include <pthread.h>

void *session(void *arg);

int main(int argc, char *argv[]){
    
    //thread field
    pid_t p_pid;
    pthread_t thread;
    int status;

    if(argc != 2){
        printf("please input a port number\n");
        return 1;
    }

    //get the parent processID
    p_pid = getpid();
    printf("session start!!\n");

    status = pthread_create(&thread,NULL,session,(void *)argv[1]);
    if(status != 0){
        //failed
        printf("failed");
    }else{
        //success
        printf("[%d]thread_id = %ld\n",p_pid,thread);
    }

    for(int i = 0;i < 10;i++){
        sleep(1);
        printf("%d\n",i+1);
    }

    return 0;
}

void *session(void *arg){
    //tcp field
    int sock;
    struct sockaddr_in sv_addr;
    int recv_size;
    char buf[32];

    sock = socket(AF_INET,SOCK_STREAM,0);
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    sv_addr.sin_port = htons(atoi(arg));

    if(connect(sock,(struct sockaddr*)&sv_addr,sizeof(sv_addr)) < 0){
        perror("connect");
    }

    if((recv_size = recv(sock,buf,sizeof(buf),0)) < 0){
        perror("recv");
        printf("recv failed\n");
    }
    printf("received message : %s\n",buf);

    close(sock);
}