#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void){
    int fd;
    struct ifreq ifr;

    //SOCK_DGRAM : Supports datagrams (connectionless, unreliable messages of a fixed maximum length)
    fd = socket(AF_INET,SOCK_DGRAM,0);
    ifr.ifr_ifru.ifru_addr.sa_family = AF_INET;

    //nバイトまでのsrcをdestにコピー
    strncpy(ifr.ifr_ifrn.ifrn_name,"eth0",IFNAMSIZ-1);
    //デバイス内の特別なファイルの値をrequestで操作する
    //file descriptor,device-dependent request code,untyped pointer to memory
    ioctl(fd,SIOCGIFADDR,&ifr);
    close(fd);

    printf("%s\n",inet_ntoa(
        ((struct sockaddr_in *)&ifr.ifr_ifru.ifru_addr) -> sin_addr));
    
    return 0;
}