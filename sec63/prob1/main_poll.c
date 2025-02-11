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

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob1$ ./poll 10 3
Writing to fd:  14 (read fd:  13)
Writing to fd:   4 (read fd:   3)
Writing to fd:  10 (read fd:   9)
poll() returned: 3
Readable:   3
Readable:   9
Readable:  13
*/

int main(int argc,char *argv[]){
    int numPipes=atoi(argv[1]);
    int (*pfds)[2];
    pfds=calloc(numPipes,sizeof(int [2]));
    if(pfds==NULL){
        perror("calloc");
        exit(1);
    }
    struct pollfd *pollFd=calloc(numPipes,sizeof(struct pollfd));
    if(pollFd==NULL){
        perror("calloc");
        exit(1);
    }

    for(int j=0;j<numPipes;j++){
        if(pipe(pfds[j])==-1){
            perror("pipe");
            exit(1);
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

    for(int j=0;j<numPipes;j++){
        pollFd[j].fd=pfds[j][0];
        pollFd[j].events=POLLIN;
    }

    int ready=poll(pollFd,numPipes,0);
    if(ready==-1){
        perror("poll");
        exit(1);
    }
    printf("poll() returned: %d\n",ready);

    for(int j=0;j<numPipes;j++){
        if(pollFd[j].revents & POLLIN){
            printf("Readable: %3d\n",pollFd[j].fd);
        }
    }
}