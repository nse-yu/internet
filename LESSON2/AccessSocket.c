#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(void){
    int sock;
    /*Create a new socket of type in domain, using
protocol. If PROTOCOL is zero, one is chosen automatically.
Returns a file descriptor for the new socket, or -1 for errors.*/
    sock = socket(AF_INET,SOCK_STREAM,0);

    if(sock < 0){
        printf("socket failed\n");
        return 1;
    }
    return 0;
}