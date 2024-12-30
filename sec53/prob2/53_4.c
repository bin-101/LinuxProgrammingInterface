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
    sem_t *sem=sem_open(argv[1],0);
    if(sem==SEM_FAILED){
        perror("sem_open");
        exit(1);
    }

    if(sem_post(sem)==-1){
        perror("sem_post");
        exit(1);
    }
}
