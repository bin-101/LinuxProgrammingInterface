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
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <mqueue.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob1$ gcc 52_2.c -o52_2
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob1$ gcc 52_4.c -o52_4
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob1$ gcc 52_5.c -o52_5
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob1$ ./52_2 -cx /mq
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob1$ ./52_4 /mq msg1 0
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob1$ ./52_5 /mq
Read 4 bytes; priority=0
msg1
正常に動くことを確認できた
*/

int main(int argc,char *argv[]){
    int flags=O_RDONLY;
    int opt;
    while((opt=getopt(argc,argv,"n"))!=-1){
        switch(opt){
            case 'n':
                flags|=O_NONBLOCK;
                break;
            default:
                perror("default");
                exit(1);
        }
    }

    int mqd=mq_open(argv[optind],flags);
    if(mqd==(mqd_t)-1){
        perror("mq_open");
        exit(1);
    }

    struct mq_attr attr;
    if(mq_getattr(mqd,&attr)==-1){
        perror("mq_getattr");
        exit(1);
    }

    void *buffer=malloc(attr.mq_msgsize);
    if(buffer==NULL){
        perror("malloc");
        exit(1);
    }

    int prio;
    int numRead=mq_receive(mqd,buffer,attr.mq_msgsize,&prio);
    if(numRead==-1){
        perror("mq_receive");
        exit(1);
    }

    printf("Read %d bytes; priority=%u\n",numRead,prio);
    if(write(STDOUT_FILENO,buffer,numRead)==-1){
        perror("write");
        exit(1);
    }
    write(STDOUT_FILENO,"\n",1);
}