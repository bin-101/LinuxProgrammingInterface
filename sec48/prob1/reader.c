#include "example48_1.h"
#include "eventflag.h"

// 写経

int main(int argc,char *argv[]){
    int semid=semget(SEM_KEY,0,0);
    if(semid==-1){
        perror("semget");
        exit(1);
    }

    int shmid=shmget(SHM_KEY,0,0);
    if(shmid==-1){
        perror("shmget");
        exit(1);
    }
    struct shmseg *shmp=shmat(shmid,NULL,SHM_RDONLY);
    if(shmp==(void *)-1){
        perror("shmat");
        exit(1);
    }

    int xfrs;
    int bytes;
    for(xfrs=0,bytes=0;;xfrs++){
        if(waitForEventFlag(semid,READ_SEM)==-1){
            perror("waitForEventFlag");
            exit(1);
        }
        // waitとclearがatomicではないのが気になるが、今回のプログラムでは問題にならない
        if(clearEventFlag(semid,READ_SEM)==-1){
            perror("clearEventFlag");
            exit(1);
        }
        if(shmp->cnt==0){
            break;
        }
        bytes+=shmp->cnt;
        if(write(STDOUT_FILENO,shmp->buf,shmp->cnt)!=shmp->cnt){
            perror("partial write");
            exit(1);
        }
        if(setEventFlag(semid,WRITE_SEM)==-1){
            perror("setEventFlag");
            exit(1);
        }
    }
    if(shmdt(shmp)==-1){
        perror("shmdt");
        exit(1);
    }
    if(setEventFlag(semid,WRITE_SEM)==-1){
        perror("setEventFlag");
        exit(1);
    }
    fprintf(stderr,"Received %d bytes (%d xfrs)\n",bytes,xfrs);
}