#define _GNU_SOURCE
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

/*
- int sigpending(sigset_t *set)
    - 返り値
        - 成功: 0
        - 失敗: -1
    - 自プロセスの保留シグナルをsetに代入する
    - sigismember()でsetに含まれるシグナルを調べる

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec33/prob1$ ./a.out
137668123952704: signal 12 is pending
137668123952704: signal 15 is pending
137668134438464: signal 10 is pending
137668134438464: signal 15 is pending
*/

void* threadFunc(void *arg){
    sleep(5);
    sigset_t set;
    int res=sigpending(&set);
    if(res){
        printf("sigpending: %s",strerror(errno));
    }
    for(int i=1;i<NSIG;i++){
        if(sigismember(&set,i)){
            printf("%ld: signal %d is pending\n",pthread_self(),i);
        }
    }
    return NULL;
}

int main(){
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_BLOCK,&set,NULL);

    pthread_t t1,t2;
    pthread_create(&t1,NULL,threadFunc,NULL);
    pthread_create(&t2,NULL,threadFunc,NULL);

    pthread_kill(t1,SIGUSR1);
    pthread_kill(t2,SIGUSR2);
    kill(getpid(),SIGTERM);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
}