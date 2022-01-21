#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#define PORT 6789

int main(int argc,char* argv[])
{
    // field
    char* hostname;
    int sock;
    int numrecv, numsend;
    struct sockaddr_in addr;
    struct hostent *hostInfo;
    char buf[256];
    
    //command line
    if(argc != 2){
        fprintf(stderr,"USAGE: %s svr_addr \n",argv[0]);
        return 1;
    }
    hostname = argv[1];

    //initialize
    memset((char*)&addr,'\0',sizeof(addr));

    //get the host informaition
    if((hostInfo = gethostbyname(hostname)) == NULL){
        printf("no host\n");
        return 1;
    }

    // define
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr =* (unsigned int*)hostInfo->h_addr_list[0];
    //↑　An array of pointers to network addresses for the host (in network byte order), terminated by a null pointer.
    
    //show ip addr
    //sin_addr is the IP host address. The s_addr member of struct in_addr contains the host interface address in network byte order. 
    printf("%s \n",inet_ntoa(addr.sin_addr));

    // create
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;
    }
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr)) < 0){
        perror("connect");
        return 1;
    }
    printf("connected to %s \n",hostname);

    while (1)
    {
        //initialize
        memset(buf, '\0', sizeof(buf));

        printf("\x1b[36m");
        printf("This side:");
        scanf("%s", buf);
        if((numsend = send(sock,buf,sizeof(buf),0)) < 0){
            perror("send");
        }
        memset(buf, '\0', numsend);

        if((numrecv = recv(sock,buf,sizeof(buf),0)) < 0){
            perror("recv");
        }
        printf("\x1b[35m");
        printf("Connected side: %s\n", buf);
        memset(buf, '\0', numrecv);
    }
    close(sock);

    return 0;
}