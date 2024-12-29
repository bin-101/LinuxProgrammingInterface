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

int main(int argc,char *argv[]){
    int flags=O_WRONLY;
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

    int prio=(argc>optind+2)?atoi(argv[optind+2]):0;
    if(mq_send(mqd,argv[optind+1],strlen(argv[optind+1]),prio)==-1){
        perror("mq_send");
        exit(1);
    }
}