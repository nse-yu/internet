#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define LEN_OF_LIST 10

void sendAll(int fdl[],char* buf,int fdl_length,int except_idx);
void listAll(int fdl[],int len);

int main(int argc,char *argv[]){
    //field
    int sock;
    struct sockaddr_in addr,new_addr;
    int newaddrSize = sizeof(new_addr);

    char buf[256];
    int nbyte;

    int fdl[LEN_OF_LIST];
    fd_set readfds;
    int ready,nfds = 0;

    //initialize
    memset((char *)&addr,'\0',sizeof(addr));
    for(int i = 0;i < LEN_OF_LIST;i++)
    {
        fdl[i] = -1;//implies it isn't accepted
    }

    //define
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;

    //create
    if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        perror("socket");
        return 1;
    }
    //bind
    if(bind(sock,(struct sockaddr*)&addr,sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }
    //prepare to connect(1 to 1 communication)
    if(listen(sock,10) < 0)
    {
        perror("listen");
        return 1;
    }

    while(1){
        //initialize
        memset(buf,'\0',sizeof(buf));
        FD_ZERO(&readfds);
        //set
        FD_SET(sock,&readfds);
        nfds = sock;

        //decide which fd is the biggest number
        for(int i = 0;i < LEN_OF_LIST;i++)
        {//accepted?
            if(fdl[i] != -1)
            {
                FD_SET(fdl[i],&readfds);
                nfds = nfds < fdl[i] ? fdl[i] : nfds;
            }
        }
        printf("--------------------------------------------\n");
        listAll(fdl,LEN_OF_LIST);

        printf("[nfds %d]waiting... -> ",nfds);
        //get count how many fd is in ready
        ready = select(nfds+1,&readfds,NULL,NULL,NULL);
        printf("<exit to select>\n");

        //Interrupt by signal
        if (ready == -1 && errno == EINTR)
        {
            printf("[continue] EINTR\n");
            continue;//return to the while starting point
        }
        //fail
        if (ready == -1) 
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        printf("%d fd in select\n",ready);

        //ready?
        if(FD_ISSET(sock,&readfds))
        {
            printf("[ISSET] sock = true\n");
            for(int i = 0;i < LEN_OF_LIST;i++)
            {//accepted?
                if(fdl[i] != -1 || !FD_ISSET(sock,&readfds))
                {
                    continue;
                }
                //not accepted -> connect (receive client's address into new_addr)
                if((fdl[i] = accept(sock,(struct sockaddr*)&new_addr,&newaddrSize)) < 0)
                {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                printf("[entry] client_%d\n",fdl[i]);
                break;
            }
        }//isset(sock)

        for(int i = 0;i < LEN_OF_LIST;i++)
        {//ready to receive from the fdl elements?
            if(fdl[i] > 0 && FD_ISSET(fdl[i],&readfds))
            {
                printf("[ready] fdl[%d] = %d\n",i,fdl[i]);

                //read from a client
                nbyte = recv(fdl[i],buf,sizeof(buf),0);
                if(nbyte <= 0)
                {
                    close(fdl[i]);
                    fdl[i] = -1;
                    continue;
                }
                printf("[recv] %s from fdl[%d] = %d\n",buf,i,fdl[i]);

                //send the received message to all fdl exclude fd which sent this message
                sendAll(fdl,buf,LEN_OF_LIST,99);
                //reinitialize
                memset(buf,'\0',sizeof(buf));
            }
        }
    }//while
    //close
    for(int i = 0;i < LEN_OF_LIST;i++){//-------------------for6
        close(fdl[i]);
    }
    close(sock);

    return 0;
}

void sendAll(int fdl[],char* buf,int fdl_legnth,int except_idx){
    int nbyte = 0;
    char new_buf[256];

    for(int i = 0;i < fdl_legnth;i++)
    {
        if(i != except_idx && fdl[i] != -1)
        {
            memset(new_buf,'\0',sizeof(new_buf));
            sprintf(new_buf,"(fd_%d) %s",fdl[i],buf);
            nbyte = send(fdl[i],new_buf,sizeof(new_buf),0);
        }
        if(nbyte == -1)
        {
            fprintf(stderr,"[failed to send] client_%d\n",fdl[i]);
            continue;
        }
    }
}

/*
void communicate(){
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
*/

void listAll(int fdl[],int len){
    for(int i = 0;i < len;i++){
        printf("%d: fdl = %d\n",i,fdl[i]);
    }
}