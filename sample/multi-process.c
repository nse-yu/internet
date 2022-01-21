#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void counter();

int main(void){
    pid_t p_pid,pid;

    //呼び出されているプロセスのIDを返す（親プロセスにあたる）
    p_pid = getpid();
    printf("[%d]start\n",p_pid);

    /*親プロセスからのfork()によって、親の正確なコピーである子プロセスを新規作成する。
    　また、子プロセスは単一のスレッドのみを使用し、親スレッドのコピーであるが、異なるスレッドIDが付く
    
      return : 正常終了時には0を子プロセスに返し、親プロセスには子プロセスのIDを返す。異常時には親に-1を返す*/

    //1つ目の子プロセス
    switch(pid = fork()){
        case 0:
            counter();
            exit(0);
        case -1:
            perror("fork");
            break;
        default:
            printf("[%d]child pid = %d\n",p_pid,pid);
            break;
    }
    //2つ目の子プロセス
    switch(pid = fork()){
        case 0:
            counter();
            exit(0);
        case -1:
            perror("fork");
            break;
        default:
            printf("[%d]child pid = %d\n",p_pid,pid);
            break;
    }
    /*wait:子プロセスが終了するまで（子が終了シグナルを投げるまで）親プロセスを停止
           正常に終了した際には、終了した子プロセスのPIDを返す。異常時には-1を返す。*/
           
    pid = wait(0);
    printf("[%d]pid = %d end\n",p_pid,pid);
    pid = wait(0);
    printf("[%d]pid = %d end\n",p_pid,pid);

    printf("[%d]end\n",p_pid);
}
void counter(){
    int i;
    pid_t pid;

    pid = getpid();
    for(i = 0;i < 10;i++){
        //1秒間のプロセス休止
        sleep(1);
        //復帰ごとに経過秒数を表示
        printf("[%d]%d\n",pid,i);
    }
}