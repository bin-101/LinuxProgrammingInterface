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
sig_speed_sigsuspend.c の写経

*/

static void handler(int sig){

}

#define TESTSIG SIGUSR1

int main(int argc,char *argv[]){
    int numSigs=atoi(argv[1]);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    sa.sa_handler=handler;
    if(sigaction(TESTSIG,&sa,NULL)==-1){
        exit(1);
    }

    sigset_t blockedMask,emptyMask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask,TESTSIG);
    if(sigprocmask(SIG_SETMASK,&blockedMask,NULL)==-1){
        exit(1);
    }
    sigemptyset(&emptyMask);

    pid_t childPid=fork();
    switch(childPid){
        case -1:
            exit(1);
        case 0:
            for(int scnt=0;scnt<numSigs;scnt++){
                if(kill(getppid(),TESTSIG)==-1)
                    exit(1);
                if(sigsuspend(&emptyMask)==-1 && errno!=EINTR)
                    exit(1);
            }
            exit(0);
        default:
            for(int scnt=0;scnt<numSigs;scnt++){
                if(sigsuspend(&emptyMask)==-1 && errno!=EINTR)
                    exit(1);
                if(kill(childPid,TESTSIG)==-1)
                    exit(1);
            }
            exit(0);
    }
}