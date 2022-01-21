#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

/*select.h概要
・select()とpselect()は、プログラムが複数のファイルディスクリプタを監視できるようにする。
　そして、１つ以上のfdがI/O可能状態「ready」になるまで待機する。
　ready状態というのは、ファイルディスクリプタが、対応するI/O処理をブロッキングなしで受けることが可能なときを言う。
　例えば、read()やwrite()など

　select()というのは、ファイルディスクリプタの番号が「FD_SETSIZE」よりも小さいものだけを監視する。※poll()にはこのような制限はない。
　select()もpselect()も、以下の３つの違いを除いてほぼ同様である。
　１．select()は、構造体timevalのsecondsやmicrosecondsを用いてタイムアウトを定めるが、pselect()は、構造体timespecのsecondsやnanosecondsを用いる。
　２．select()は、経過時間を把握するためにタイムアウトの引数を更新する可能性がある。
　３．select()には、引数sigmaskがなく、これはpselectのsigmask=NULLと同様の動きをなす。

　監視されるファイルディスクリプタの種類は３つ。
　　readfds　：文字が読み込み可能になるかどうかを監視される（厳密には、読み込みがブロックされないかの監視）
    writefds ：スペースへの書き込みが可能かどうかを監視される（大きな書き込みはブロックされる可能性もある）
    exceptfds：例外的な状況の監視をされる

　ファイルディスクリプタの登録には、４つのマクロが提供されている。
    FD_ZERO()　　　　 ：セットをクリアする
    FD_SET(),FD_CLR() ：追加、削除
    FD_ISSET()　　　　：ファイルディスクリプタがセットの一部か（まだ監視されているか）をテストする;select()が戻ったときに役立つ
    
　どうやら、最初にやるべきことは、監視対象を保持するための「集合（セット）」をFD_ZEROでクリア（初期化）することらしい*/
int main(){
    int sock1,sock2;
    struct sockaddr_in addr1,addr2;
    fd_set fds,readfds;
    char buf[2048];
    int maxfd;

    //socketを２つ用意
    sock1 = socket(AF_INET,SOCK_DGRAM,0);
    sock2 = socket(AF_INET,SOCK_DGRAM,0);

    if((sock1 < 0) | (sock2 < 0)){
        perror("socket");
        exit(-1);
    }

    //２つのアドレスを定義
    addr1.sin_family = AF_INET;
    addr2.sin_family = AF_INET;
    addr1.sin_addr.s_addr = INADDR_ANY;
    addr2.sin_addr.s_addr = INADDR_ANY;
    addr1.sin_port = htons(11111);
    addr2.sin_port = htons(22222);

    if(bind(sock1,(struct sockaddr*)&addr1,sizeof(addr1)) != 0){
        perror("bind1");
        exit(-1);
    }
    if(bind(sock2,(struct sockaddr*)&addr2,sizeof(addr2)) != 0){
        perror("bind2");
        exit(-1);
    }

    //まず、保護対象のディスクリプタを入れるためのセット->readfdsを宣言&FD_ZEROで初期化
    //readfdsなので、読み込みの可否を監視するという意味で宣言している
    FD_ZERO(&readfds);//セットの初期化

    //sock1,2をreadfds監視対象としてセットに含めるといった意味でいいだろう
    FD_SET(sock1,&readfds); //set sock1 as read sock for select
    FD_SET(sock2,&readfds); //set sock2 as read sock for select

    //documentによると、「nfdsには、特定のセットにおけるディスクリプタの最大数値＋1を指定すること」と記載されている。
    //このnfdsを上限として、その上限までディスクリプタにチェック（監視）が入るらしい。
    if(sock1 > sock2){
        maxfd = sock1;
    }else{
        maxfd = sock2;
    }
    while(1){
        //何でか知らんけど、readfdsの内容をfdsにコピーしている。
        memcpy(&fds,&readfds,sizeof(fd_set));
        //上記の理由により、maxfd+1を指定している。memcpyにより、fdsをここに入れる
        select(maxfd+1,&fds,NULL,NULL,NULL);

        //isset：監視対象がreadyのときに0以外
        //read from sock1
        if(FD_ISSET(sock1,&fds)){
            memset(buf,'\0',sizeof(buf));
            recv(sock1,buf,sizeof(buf),0);
            printf("%s\n",buf);
        }
        //read from sock2
        if(FD_ISSET(sock2,&fds)){
            memset(buf,'\0',sizeof(buf));
            recv(sock2,buf,sizeof(buf),0);
            printf("%s\n",buf);
        }
    }
    close(sock1);
    close(sock2);

    return 0;
}