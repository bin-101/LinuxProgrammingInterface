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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob5$ ../prob4/create # セマフォ作成
semid: 16
pid: 71175

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob5$ ./event clear 16 0 # clear(1をセット)
0
Success

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob5$ ./event wait 16 0 & # set(0)になるまで待つ
[1] 71541

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob5$ ./event get 16 0 & # get
[2] 71559
1 # 現在の値は1
Success
[2]+  終了                  ./event get 16 0

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob5$ ./event set 16 0 # set(0をセット)
0
Success
0
Success
[1]+  終了                  ./event wait 16 0 # waitが終了

*/

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


int main(int argc,char *argv[]){
    int semId=atoi(argv[2]);
    int semNum=atoi(argv[3]);
    int ret=100;
    if(strcmp(argv[1],"set")==0){
        ret=setEventFlag(semId,semNum);
    }else if(strcmp(argv[1],"clear")==0){
        ret=clearEventFlag(semId,semNum);
    }else if(strcmp(argv[1],"wait")==0){
        ret=waitForEventFlag(semId,semNum);
    }else if(strcmp(argv[1],"get")==0){
        ret=getFlagState(semId,semNum);
    }
    printf("%d\n",ret);
    printf("%s\n",strerror(errno));
}