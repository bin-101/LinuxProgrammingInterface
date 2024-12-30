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
#include <semaphore.h>

int main(int argc,char *argv[]){
    int opt,flags;
    while((opt=getopt(argc,argv,"cx"))!=-1){
        switch(opt){
            case 'c':
                flags|=O_CREAT;
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
    int value=(argc<=optind+1)?0:atoi(argv[optind+1]);
    sem_t *sem=sem_open(argv[optind],flags,perms,value);
    if(sem==SEM_FAILED){
        perror("sem_open");
        exit(1);
    }
}
