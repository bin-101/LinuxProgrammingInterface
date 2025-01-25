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

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec22/prob3$ time ./sigwaitinfo 1000000

real    0m11.556s
user    0m0.303s
sys     0m5.114s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec22/prob3$ time ./sigsuspend 1000000

real    0m14.819s
user    0m0.378s
sys     0m6.564s
*/

#define TESTSIG SIGUSR1

int main(int argc,char *argv[]){
    int numSigs=atoi(argv[1]);

    sigset_t blockedMask,fillMask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask,TESTSIG);
    if(sigprocmask(SIG_SETMASK,&blockedMask,NULL)==-1){
        exit(1);
    }
    sigfillset(&fillMask);

    pid_t childPid=fork();
    siginfo_t info;
    switch(childPid){
        case -1:
            exit(1);
        case 0:
            for(int scnt=0;scnt<numSigs;scnt++){
                if(kill(getppid(),TESTSIG)==-1)
                    exit(1);
                if(sigwaitinfo(&fillMask, &info)==-1 && errno!=EINTR)
                    exit(1);
            }
            exit(0);
        default:
            for(int scnt=0;scnt<numSigs;scnt++){
                if(sigwaitinfo(&fillMask,&info)==-1 && errno!=EINTR)
                    exit(1);
                if(kill(childPid,TESTSIG)==-1)
                    exit(1);
            }
            exit(0);
    }
}