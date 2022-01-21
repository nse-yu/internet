#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *counter(void *arg);

int main(void){
    pid_t p_pid;
    pthread_t thread_id1,thread_id2;
    int status;
    void *result;

    //parent's pid
    p_pid = getpid();
    printf("[%d]start\n",p_pid);

    //呼び出しプロセスから新しいスレッドを生成する。
    /*
    第一引数には、新しいスレッドのIDへのポインタが格納され、以降の参照において使用する。
    第二引数は、pthread_attr_t 構造体へのポインターであり、 pthread_attr_t 構造体の内容を使用して、
    スレッド作成時に 新しいスレッドの属性が決定される。NULLの場合には、デフォルトの属性で作成される。
    第三引数は、新しいスレッドで実行する関数へのポインタ
    return : 正常時には0を返し、異常時にはエラーナンバーを返す。
    */
    status = pthread_create(&thread_id1,NULL,counter,(void *)NULL);
    if(status != 0){
        fprintf(stderr,"pthread_create : %s",strerror(status));
    }else{
        printf("[%d]thread_id1 = %ld\n",p_pid,thread_id1);
    }

    status = pthread_create(&thread_id2,NULL,counter,(void *)NULL);
    if(status != 0){
        fprintf(stderr,"pthread_create : %s",strerror(status));
    }else{
        printf("[%d]thread_id2 = %ld\n",p_pid,thread_id2);
    }

    //第一引数で与えられたスレッドの終了を待ち、第二引数に終了ステータスが格納される。
    pthread_join(thread_id1,&result);
    printf("[%d]thread_id1 = %ld end\n",p_pid,thread_id1);
    pthread_join(thread_id2,&result);
    printf("[%d]thread_id2 = %ld end\n",p_pid,thread_id2);

    printf("[%d]end\n",p_pid);
}

void *counter(void *arg){
    int i;
    pid_t pid;
    pthread_t thread_id;

    pid = getpid();
    //現在のスレッドのIDを返す
    thread_id = pthread_self();

    for(i = 0;i < 10;i++){
        //1秒間のプロセス休止
        sleep(1);
        //復帰ごとに経過秒数を表示
        printf("[%d][%ld]%d\n",pid,thread_id,i);
    }
}