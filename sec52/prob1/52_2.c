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
    struct mq_attr attr,*attrp;
    attrp=NULL;
    attr.mq_maxmsg=10;
    attr.mq_msgsize=2048;
    int flags=O_RDWR;

    int opt;
    while((opt=getopt(argc,argv,"cm:s:x"))!=-1){
        switch(opt){
            case 'c':
                flags|=O_CREAT;
                break;
            case 'm':
                attr.mq_maxmsg=atoi(optarg);
                attrp=&attr;
                break;
            case 's':
                attr.mq_msgsize=atoi(optarg);
                attrp=&attr;
                break;
            case 'x':
                flags|=O_EXCL;
                break;
            default:
                perror("default");
                exit(1);
        }
    }

    int perms=(S_IRUSR|S_IWUSR);
    int mqd=mq_open(argv[optind],flags,perms,attrp);
    if(mqd==(mqd_t)-1){
        perror("mq_open");
        exit(1);
    }
    
}