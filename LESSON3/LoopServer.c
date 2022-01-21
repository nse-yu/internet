#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void){
    int desc,len,new_desc;
    /**netinet_in.hに所属する構造体フィールド
     * socketのアドレスを示すための構造体*/
    /*The sockaddr_in structure is used to store addresses for the
       Internet address family.  Pointers to this type shall be cast by
       applications to struct sockaddr * for use with socket functions.
       The sin_port and sin_addr members shall be in network byte order.*/
    struct sockaddr_in addr;
    struct sockaddr_in client;
    /*
    The <netinet/in.h> header shall define the following symbolic
       constant for use as a local address in the structure passed to
       bind():

       INADDR_ANY      IPv4 wildcard address.
    */
    
    //create the socket
    desc = socket(AF_INET,SOCK_STREAM,0);
    //domain
    addr.sin_family = AF_INET;
    //port number
    addr.sin_port = htons(12345);
    //ip address
    addr.sin_addr.s_addr = INADDR_ANY;
    //filedescripterにlocal addressを結び付ける
    /* arg3 : addrlen specifies the size, in bytes, of the ad‐
       dress  structure  pointed to by addr.*/
    bind(desc,(struct sockaddr*)&addr,sizeof(addr));

    /*Prepare to accept connections on socket FD.
      N connection requests will be queued before further requests are refused.
      Returns 0 on success, -1 for errors.

      リクエストが来るまではここまでで待つことになる
    */
   //arg2 : 許容するクエリの数
    listen(desc,5);

    //------------------------------------accept request
    len = sizeof(client);
    /*wait a connection on socket FD.
      When a connection arrives, open a new socket to communicate with it,
      set *ADDR (which is *ADDR_LEN bytes long) to the address of the connecting
      peer and *ADDR_LEN to the address's actual length, and return the
      new socket's descriptor, or -1 for errors.
      This function is a cancellation point and therefore not marked with
      __THROW.*/
    new_desc = accept(desc,(struct sockaddr*)&client,&len);

    //send 5 characters
    /*ssize_t send(int sockfd, const void *buf, size_t len, int flags);*/
    send(new_desc,"Hello",5,0);

    //close connection
    close(desc);
    close(new_desc);

    return 0;
}