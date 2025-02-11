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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob1$ ./select 10 3
Writing to fd:  22 (read fd:  21)
Writing to fd:  22 (read fd:  21)
Writing to fd:  14 (read fd:  13)
poll() returned: 2
Readable:  13
Readable:  21
*/

int main(int argc,char *argv[]){
    int numPipes=atoi(argv[1]);
    int (*pfds)[2];
    pfds=calloc(numPipes,sizeof(int [2]));
    if(pfds==NULL){
        perror("calloc");
        exit(1);
    }

    fd_set readfds;
    FD_ZERO(&readfds);
    int max_fd=-1;

    for(int j=0;j<numPipes;j++){
        if(pipe(pfds[j])==-1){
            perror("pipe");
            exit(1);
        }
        FD_SET(pfds[j][0],&readfds);
        if(pfds[j][0]>=max_fd){
            max_fd=pfds[j][0]+1;
        }
    }

    int numWrites=1;
    if(argc>2) numWrites=atoi(argv[2]);

    srandom((int)time(NULL));
    for(int j=0;j<numWrites;j++){
        int randPipe=random()%numPipes;
        printf("Writing to fd: %3d (read fd: %3d)\n",pfds[randPipe][1],pfds[randPipe][0]);
        if(write(pfds[randPipe][1],"a",1)==-1){
            perror("write");
            exit(1);
        }
    }

    struct timeval timeout;
    timeout.tv_sec=0;
    timeout.tv_usec=0;
    int ready=select(max_fd,&readfds,NULL,NULL,&timeout);
    if(ready==-1){
        perror("poll");
        exit(1);
    }
    printf("poll() returned: %d\n",ready);

    for(int j=0;j<numPipes;j++){
        if(FD_ISSET(pfds[j][0],&readfds)){
            printf("Readable: %3d\n",pfds[j][0]);
        }
    }
}