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
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <limits.h>
#include <stddef.h>

// 演習47-4からコピー

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

typedef enum { FALSE, TRUE } Boolean;

Boolean bsUseSemUndo=FALSE;
Boolean bsRetryOnEintr=TRUE;

int initSemAvailable(int semId,int semNum){
    union semun arg;
    arg.val=1;
    return semctl(semId,semNum,SETVAL,arg);
}

int initSemInUse(int semId,int semNum){
    union semun arg;
    arg.val=0;
    return semctl(semId,semNum,SETVAL,arg);
}

// -1
int reserveSem(int semId,int semNum){
    struct sembuf sops;
    sops.sem_num=semNum;
    sops.sem_op=-1;
    sops.sem_flg=bsUseSemUndo?SEM_UNDO:0;
    while(semop(semId,&sops,1)==-1){
        if(errno!=EINTR || !bsRetryOnEintr){
            return -1;
        }
    }
    return 0;
}

// +1
int releaseSem(int semId,int semNum){
    struct sembuf sops;
    sops.sem_num=semNum;
    sops.sem_op=1;
    sops.sem_flg=bsUseSemUndo?SEM_UNDO:0;
    return semop(semId,&sops,1);
}

// 条件付き獲得
int reserveSemNB(int semId,int semNum){
    struct sembuf sops;
    sops.sem_num=semNum;
    sops.sem_op=-1;
    sops.sem_flg=bsUseSemUndo?SEM_UNDO:0;
    sops.sem_flg|=IPC_NOWAIT;
    return semop(semId,&sops,1);
}
