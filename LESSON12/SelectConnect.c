#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){//-----------------------------------------------------------main
    int fd;
    int fdl[10];
    struct servent *serv;
    struct sockaddr_in addr,rcvaddr;
    int len;
    fd_set fds,readfds;
    char buf[2048];
    int cnt;

    //initialize
    memset(buf,'\0',sizeof(buf));
    //fdl[10]まで-1が入る
    for(int i = 0; i < sizeof(fdl)/sizeof(fdl[0]);i++){//------------------------------------------for1
        fdl[i] = -1; //total mem size of list / mem size per data = length of list
    }//-------------------------------------------------------------------------------------------e_for1
    len = sizeof(rcvaddr);
    memset(&addr,'\0',sizeof(addr));
    memset(&rcvaddr,'\0',sizeof(rcvaddr));

    //socket open
    if((fd = socket(AF_INET,SOCK_DGRAM,0)) < 0){//---------------------------------------------if1
        perror("socket");
        return -1;
    }//---------------------------------------------------------------------------------------e_if1
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(50000);
    addr.sin_addr.s_addr = INADDR_ANY;

    //socketの確立
    if(bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0){//---------------------------------------------------if2
        perror("bind");
        return -1;
    }//------------------------------------------------------------------------------------------------------e_if2
    //接続許容数を設定
    if(listen(fd,1) < 0){//--------------------------------------------------------------if3
        perror("listen");
        return -1;
    }//---------------------------------------------------------------------------------e_if3

    int maxfd;//max number of fd
    fd_set rfds;//for read
    struct timeval tv;//timeout

    while(1){//----------------------------------------------------------------------------------------------------------------while
        //set all fd select
        FD_ZERO(&rfds);//initialize
        FD_SET(fd,&rfds);//set fd read monitor set
        maxfd = fd;//受け入れが１つだけだから、maxは確定でfd

        for(int i = 0;i < sizeof(fdl)/sizeof(fdl[0]);i++){//----------------------------------------------for2
            //accept（接続）が成功したものだけを監視対象（読み込み可能性の監視）とする。
            if(fdl[i] != -1){//--------------------------------------------------------------if4
                FD_SET(fdl[i],&rfds);
                //常にmaxfdは最大であることが必要なので、for文でついでに更新する
                if(fdl[i] > maxfd){//--------------------------------------if5
                    maxfd = fdl[i];
                }//-------------------------------------------------------e_if5
            }//----------------------------------------------------------------------------e_if4
        }//---------------------------------------------------------------------------------------------e_for2
        //set timeout value
        tv.tv_sec = 10;
        tv.tv_usec = 500000;

        //check fd : return readyなディスクリプタの番号を返す
        cnt = select(maxfd+1,&rfds,NULL,NULL,&tv);

        //-1はただのエラー
        if(cnt < 0){//--------------------------------------------if6
        //EINTR:関数割り込みエラー
            if(errno == EINTR){//-------------------------if7
            //次のwhileループ
                continue;
            }//-----------------------------------------e_if7
            //それ以外のエラーは完全に処理終了（socketを閉じるためにendへ行く）
            goto end;
        }else if(cnt == 0){//タイムアウト
            continue;
        }else{//ready状態
            if(FD_ISSET(fd,&rfds)){//----------------------------------------------------------------------------if8
                for(int i = 0;i < sizeof(fdl)/sizeof(fdl[0]);i++){//-------------------------------------for3
                //初期状態では全ての要素が-1
                /*fdlは、acceptできたsocketを最大10個格納するためのリスト。初期状態を-1とし、成功したら書き換える。*/
                    if(fdl[i] == -1){//----------------------------------------------------------if9
                    //return new socket fd (if error, return -1. else, may >0)
                        if((fdl[i] = accept(fd,(struct sockaddr*)&rcvaddr,&len)) < 0){//--if10
                        //１つでもエラーを吐いたら、処理終了（socketを閉じるためにendへ行く）
                            goto end;
                        }//-------------------------------------------------------------e_if10
                        //エラー吐いてないなら、acceptできたsocketの番号を表示して、for3を終わる
                        fprintf(stdout,"socket:%d connected. \n",fdl[i]);
                        break;
                    }//------------------------------------------------------------------------e_if9
                }//------------------------------------------------------------------------------------e_for3
            }//-------------------------------------------------------------------------------------------------e_if8
            
            //accept（接続）し、監視されているfdに対する処理
            for(int i = 0;i < sizeof(fdl)/sizeof(fdl[0]);i++){//---------------------------for4
            //読み込み可能
                    if(FD_ISSET(fdl[i],&rfds)){//---------------------------if11
                        cnt = recv(fdl[i],buf,sizeof(buf),0);
                        //成功
                        if(cnt > 0){//---------------------------if12
                            fprintf(stdout,"recv:\"%s\"\n",buf);
                        }else if(cnt == 0){//接続途切れ->not accept状態を示す-1で書き換え
                            fprintf(stdout,"socket:%d disconnected. \n",fdl[i]);
                            close(fdl[i]);
                            fdl[i] = -1;
                        }else{//失敗したら処理終了
                            goto end;
                        }//------------------------------------e_if12
                    }//---------------------------------------------------e_if11
                }//---------------------------------------------------------------------e_for4
        }//------------------------------------------------------e_if6
    }//--------------------------------------------------------------------------------------------e_while
end:
//処理を終えた（エラーで飛ばされたものも）やつらは、最後に開いたsocketを閉じてから終了
    for(int i = 0;i < sizeof(fdl)/sizeof(fdl[0]);i++){//-------------------for6
        close(fdl[i]);
    }//------------------------------------------------------------------e_for6
    close(fd);
    return 0;
}//--------------------------------------------------------e_main