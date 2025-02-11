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

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob5$ gcc main_poll.c -opoll
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob5$ ./poll - 0
f
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob5$ f
f: コマンドが見つかりません
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob5$ ./poll - 0
^CA signal was caught
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

    int timeout;
    if(strcmp(argv[1],"-")==0){
        timeout=-1;
    }else{
        timeout=atoi(argv[1]);
    }

    int nfds=argc-2+1;
    struct pollfd *pollFd=calloc(nfds,sizeof(struct pollfd));
    for(int j=2;j<argc;j++){
        int fd=atoi(argv[j]);
        if(fd>=FD_SETSIZE){
            perror("fd>=FD_SETSIZE");
            exit(1);
        }
        if(fd>=nfds) nfds=fd+1;
        pollFd[j-2].fd=fd;
        pollFd[j-2].events=POLLIN;
    }

    if(pipe(pfd)==-1){
        perror("pipe");
        exit(1);
    }
    pollFd[argc-2].fd=pfd[0];
    pollFd[argc-2].events=POLLIN;

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
    while((ready=poll(pollFd,nfds,timeout))==-1 && errno==EINTR){
        continue;
    }
    if(ready==-1){
        perror("select");
        exit(1);
    }

    for(int i=0;i<nfds;i++){
        if(pollFd[i].revents & POLLIN){
            if(pollFd[i].fd==pfd[0]){
                printf("A signal was caught\n");
            }
        }
    }


}