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
#include <stdarg.h>
#include <signal.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec27/prob6$ ./a.out
pid: 150169
sig: 17 Child exited # SIGCHILDを受信する

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec27/prob6$ ./a.out s
bin101
res: 0
pid: -1 # waitはできない。system関数の中で行っていそう。
sig: 17 Child exited # SIGCHILDを受信する
*/

void handler(int sig){
    printf("sig: %d %s\n",sig,strsignal(sig));
}

int main(int argc,char *argv[]){
    struct sigaction sa;
    sa.sa_handler=handler;
    if(sigaction(SIGCHLD,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }
    
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset,SIGCHLD);
    if(sigprocmask(SIG_BLOCK,&sigset,NULL)==-1){
        perror("sigprocmask");
        exit(1);
    }

    if(argc>=2 && argv[1][0]=='s'){
        int res=system("whoami");
        printf("res: %d\n",res);
    }else{
        if(fork()==0){
            exit(0);
        }
    }

    pid_t pid=wait(NULL);
    printf("pid: %d\n",pid);

    if(sigprocmask(SIG_UNBLOCK,&sigset,NULL)==-1){
        perror("sigprocmask");
        exit(1);
    }


}