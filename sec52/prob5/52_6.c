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
        if(mq_notify(mqd,&sev)==-1){
            perror("mq_notify");
            exit(1);
        }
以上をコメントアウトする前
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob5$ gcc ../prob1/52_2.c -o52_2
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob5$ gcc ../prob1/52_4.c -o52_4
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob5$ ./52_4 /mq msg1 10
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob5$ ./52_4 /mq msg2 0
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob5$

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob5$ ./52_6 /mq
Read 4 bytes
Read 4 bytes # 2つ受け取った

コメントアウト後
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob5$ ./52_6 /mq
Read 4 bytes # 同じことを行ったが、1回しか受け取らなかった
*/

#define NOTIFY_SIG SIGUSR1

static void handler(int sig){

}

int main(int argc,char *argv[]){
    int mqd=mq_open(argv[1],O_RDONLY|O_NONBLOCK);
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

    sigset_t blockMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask,NOTIFY_SIG);
    if(sigprocmask(SIG_BLOCK,&blockMask,NULL)==-1){
        perror("sigprocmask");
        exit(1);
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    sa.sa_handler=handler;
    if(sigaction(NOTIFY_SIG,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }

    struct sigevent sev;
    sev.sigev_notify=SIGEV_SIGNAL;
    sev.sigev_signo=NOTIFY_SIG;
    if(mq_notify(mqd,&sev)==-1){
        perror("mq_notify");
        exit(1);
    }

    sigset_t emptyMask;
    sigemptyset(&emptyMask);
    while(1){
        sigsuspend(&emptyMask);
        // if(mq_notify(mqd,&sev)==-1){
        //     perror("mq_notify");
        //     exit(1);
        // }

        int numRead;
        while((numRead=mq_receive(mqd,buffer,attr.mq_msgsize,NULL))>=0){
            printf("Read %ld bytes\n",(long)numRead);
        }

        if(errno!=EAGAIN){
            perror("mq_receive");
            exit(1);
        }
    }
}