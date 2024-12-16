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

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob4$ ./create # セマフォ作成
semid: 15
pid: 69646

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob4$ ./binary i1 15 0 # 1をセット
Success

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob4$ ./binary r0 15 0 # -1
Success

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob4$ ./binary nb 15 0 # -1をノンブロッキングで実行
Resource temporarily unavailable

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob4$ ./binary r0 15 0 # -1を実行
^C # 止まる
*/

// 例47-9,47-10を参考にした

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

int main(int argc,char *argv[]){
    int semId=atoi(argv[2]);
    int semNum=atoi(argv[3]);
    if(strcmp(argv[1],"i1")==0){
        initSemAvailable(semId,semNum);
    }else if(strcmp(argv[1],"i0")==0){
        initSemInUse(semId,semNum);
    }else if(strcmp(argv[1],"r0")==0){
        reserveSem(semId,semNum);
    }else if(strcmp(argv[1],"r1")==0){
        releaseSem(semId,semNum);
    }else if(strcmp(argv[1],"nb")==0){
        reserveSemNB(semId,semNum);
    }
    printf("%s\n",strerror(errno));
}