#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in addr;
    int n;
    char *deststr;
    unsigned int **addrptr;
    char buf[32];

    //引数が２つ（プログラム名, URL）ではないとき
    if(argc != 2){
        printf("Usage : %s dest\n",argv[0]);
        return 1;
    }
    //urlからアドレスを取得するため、urlを変数に格納
    deststr = argv[1];

    //create empty socket
    sock = socket(AF_INET,SOCK_STREAM,0);

    //address field
    addr.sin_addr.s_addr = inet_addr(deststr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);

    /*inet_addrは、無効なアドレスが渡されたときに-1を返すが、この-1は255.255.255.255（0xffffffff）として有効なアドレスに解釈されてしまうため、
    ここで捕捉しておく必要がある。*/
    //無効なアドレスが渡されたとき
    if(addr.sin_addr.s_addr == 0xffffffff){
        //variable for host information
        struct hostent *host;

        //The gethostbyname() function is used to retrieve information about a host.
        //arg : ホスト名を含む文字列へのポインタ
        host = gethostbyname(deststr);

        //NULL -> unsuccessful
        /*p    -> pointer to struct hostent 
        (including the host information:
            name        : pointer to name of the host
            aliase      : NULL終端文字列リストへのポインタ
            addrtype    : address type of the host (ex. AF_INET)
            length      : address length
            addr_list   : バイトオーダーで表されたホストのネットワークアドレスを指すリストへのポインタ
        )
        */
        if(host == NULL){
            /*h_errono : error number
            (
                HOST_NOT_FOUND : the specified host is unknown 
                NO_DATA        : name is valid but doesn't have IP address
                NO_RECOVERY    : server error
                TRY_AGAIN      : server error
            )
            */
            if(h_errno == HOST_NOT_FOUND){
                //指定した（けどホストが見つからなかった）アドレスを表示
                printf("host not found : %s\n",deststr);
            }else{
                //それ以外のエラーについては、error numberとアドレスを表示して終わり
                printf("%s : %s\n",hstrerror(h_errno),deststr);
            }
            return 1;
        }
        //hostがとりあえず取得できていたら、hostが保持するアドレスへのポインタ(h_addr_list)へのポインタを得る
        addrptr = (unsigned int **)host->h_addr_list;

        //whileを経て、改めてh_addr_listが何も指していないときにはconnect失敗
        if(*addrptr == NULL){
            perror("connect");
            return 1;
        }

        //h_addr_listがどこかへのアドレスを指しているなら
        while(*addrptr != NULL){
            //指しているアドレスでの値を取得
            addr.sin_addr.s_addr = *(*addrptr);
            //それを文字列にして表示
            printf("addr:%s \n",inet_ntoa(addr.sin_addr));

            //return 0 on success
            //一度でも成功したなら繰り返しは終わり
            if(connect(sock,(struct sockaddr *)&addr,sizeof(addr)) == 0){
                printf("successfully connected\n");
                break;
            }
            //listに複数アドレスがあることを想定し、次のアドレスへポインタし、上と同様の処理をする
            addrptr++;
        }
        
    }else{
        //特に無効アドレスが渡されなかった時にはそのままconnectする
        if(connect(sock,(struct sockaddr *)&addr,sizeof(addr)) != 0){
            perror("connect");
            return 1;
        }
    }
    //格納用bufferを準備
    memset(buf,'\0',sizeof(buf));
    //bufferのサイズ分のみ、引数の文字列をbufに格納する
    snprintf(buf,sizeof(buf),"GET / HTTP/1.0\r\n\r\n");

    //送信できたバイト数を取得し、失敗しているならばsockへのwriteエラー
    if((n = send(sock,buf,strlen(buf),0)) < 0){
        perror("write");
        return 1;
    }
    printf("n byte = %d buf.strlen = %ld\n",n,strlen(buf));
    //bufferのサイズ分ずつ読み込むから、繰り返しが必要
    while(n > 0){
        //bufferを最初から準備する
        memset(buf,'\0',sizeof(buf));
        //さっき送信先だったsockからデータをbufferに受け取ると同時に、読み込んだバイト数を得る
        n = recv(sock,buf,sizeof(buf),0);
        
        //何もsockから読み込めなかったときはreadエラーで終わり
        if(n < 0){
            perror("read");
            return 1;
        }
        //特に問題なければ、内容を表示
        fprintf(stdout,"%s \n",buf);
    }
    close(sock);
    return 0;    
}
