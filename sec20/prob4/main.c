#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/inotify.h>

/*
int siginterrupt(int sig, int flag); flagが0の場合、実行を再開する
    成功時: 0
    エラー時: -1

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec20/prob4$ ./a.out
^Csignal: 2
^Csignal: 2
abc
nread: 4
error: Success
^Csignal: 2
nread: -1
error: Interrupted system call
*/

int my_siginterrupt(int sig,int flag){
    struct sigaction sa;
    if(sigaction(sig,NULL,&sa) == -1){ //現在のシグナルハンドラを取得
        return -1;
    }
    if(flag){
        sa.sa_flags &= ~SA_RESTART;
    }else{
        sa.sa_flags |= SA_RESTART;
    }
    if(sigaction(sig,&sa,NULL) == -1){
        return -1;
    }
    return 0;
}

void print_sig(int sig){
    char c[100];
    snprintf(c, 100, "signal: %d\n", sig);
    write(STDOUT_FILENO, c, strlen(c));
}

int main(){
    char buf[100];
    struct sigaction sa;
    sa.sa_handler = print_sig;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);

    int nread=read(STDIN_FILENO,buf,100); // SIGINTを送る
    printf("nread: %d\n",nread);
    printf("error: %s\n",strerror(errno));

    my_siginterrupt(SIGINT,1); // SA_RESTARTを解除
    nread=read(STDIN_FILENO,buf,100); // SIGINTを送る
    printf("nread: %d\n",nread);
    printf("error: %s\n",strerror(errno));
}