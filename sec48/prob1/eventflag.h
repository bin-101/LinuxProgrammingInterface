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

// 演習47-5からコピー

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

typedef enum { FALSE, TRUE } Boolean;

Boolean bsUseSemUndo=FALSE;
Boolean bsRetryOnEintr=TRUE;

// clear: 1 set:0
// waitForEventFlag(setされるまで待つ)を実装するために、setを0にする(sem_op=0で実現できる)

int setEventFlag(int semId,int semNum){
    union semun arg;
    arg.val=0;
    return semctl(semId,semNum,SETVAL,arg);
}

int clearEventFlag(int semId,int semNum){
    union semun arg;
    arg.val=1;
    return semctl(semId,semNum,SETVAL,arg);
}

int waitForEventFlag(int semId,int semNum){
    struct sembuf sops;
    sops.sem_num=semNum;
    sops.sem_op=0;
    sops.sem_flg=bsUseSemUndo?SEM_UNDO:0;
    return semop(semId,&sops,1);
}

int getFlagState(int semId,int semNum){
    union semun arg;
    return semctl(semId,semNum,GETVAL,arg);
}