#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#define PORT 6789

int main(void){
    //field
    int sock,new_sock;
    int numrecv,numsend;
    struct sockaddr_in addr,new_addr;
    int newaddrSize = sizeof(new_addr);
    char buf[256];

    while(1){
        memset((char *)&addr,'\0',sizeof(addr));
        //define
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = INADDR_ANY;

        //create
        if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0){
            perror("socket");
            return 1;
        }
        //bind
        if(bind(sock,(struct sockaddr*)&addr,sizeof(addr)) < 0){
            perror("bind");
            return 1;
        }
        //prepare to connect(1 to 1 communication)
        if(listen(sock,1) < 0){
            perror("listen");
            return 1;
        }
        printf("Waiting connection ...");

        //waiting and catch
        if((new_sock = accept(sock,(struct sockaddr*)&new_addr,&newaddrSize)) < 0){
            perror("accept");
            return 1;
        }

        while(1){
            //receive from the new socket
            if((numrecv = recv(new_sock,buf,sizeof(buf),0)) < 0){
                perror("recv");
            }
            //it means 1b in 16 basenum, so it's ESC.(refer to ASCII code table)
            //change the color in the console
            printf("\x1b[36m");
            printf("Connected side: %s\n",buf);
            //受け取ったバイト分だけnull終端埋め
            memset(buf,'\0',numrecv);
            //also change the color in the console.
            printf("\x1b[35m");
            printf("This side:");
            scanf("%s",buf);
            if((numsend = send(new_sock,buf,sizeof(buf),0) < 0) < 0){
                perror("send");
            }
            memset(buf,'\0',numsend);
        }
    }
    close(sock);
    close(new_sock);

    return 0;
}