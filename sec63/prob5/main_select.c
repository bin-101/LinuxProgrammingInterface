#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>
#include <sys/select.h>

// 例63-9とself_pipe.cを参考にした

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob5$ gcc main_select.c -oselect
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob5$ ./select 3 0
^CA signal was caught
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob5$ ./select 3 0
g
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob5$ g
g: コマンドが見つかりません
*/

int pfd[2];

void handler(int sig){
    int savedErrno=errno;
    if(write(pfd[1],"x",1)==-1 && errno!=EAGAIN){
        perror("write");
        exit(1);
    }
    errno=savedErrno;
}

int main(int argc,char *argv[]){

    struct timeval timeout;
    struct timeval *pto;
    if(strcmp(argv[1],"-")==0){
        pto=NULL;
    }else{
        pto=&timeout;
        timeout.tv_sec=atoi(argv[1]);
        timeout.tv_usec=0;
    }

    int nfds=0;
    fd_set readfds;
    FD_ZERO(&readfds);
    for(int j=2;j<argc;j++){
        int fd=atoi(argv[j]);
        if(fd>=FD_SETSIZE){
            perror("fd>=FD_SETSIZE");
            exit(1);
        }
        if(fd>=nfds) nfds=fd+1;
        FD_SET(fd,&readfds);
    }

    if(pipe(pfd)==-1){
        perror("pipe");
        exit(1);
    }
    FD_SET(pfd[0],&readfds);
    if(nfds<=pfd[0]) nfds=pfd[0]+1;

    int flags=fcntl(pfd[0],F_GETFL);
    if(flags==-1){
        perror("fcntl");
        exit(1);
    }
    flags|=O_NONBLOCK;
    if(fcntl(pfd[0],F_SETFL,flags)==-1){
        perror("F_SETFL");
        exit(1);
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    sa.sa_handler=handler;
    if(sigaction(SIGINT,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }

    int ready;
    while((ready=select(nfds,&readfds,NULL,NULL,pto))==-1 && errno==EINTR){
        continue;
    }

    if(ready==-1){
        perror("select");
        exit(1);
    }

    if(FD_ISSET(pfd[0],&readfds)){
        printf("A signal was caught\n");
        for(;;){
            char ch;
            if(read(pfd[0],&ch,1)==-1){
                if(errno==EAGAIN){
                    break;
                }else{
                    perror("read");
                    exit(1);
                }
            }
        }
    }


}