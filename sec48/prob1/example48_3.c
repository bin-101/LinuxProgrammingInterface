#include "example48_1.h"
#include "binary_sems.h"

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
        if(reserveSem(semid,READ_SEM)==-1){
            perror("reserveSem");
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
        if(releaseSem(semid,WRITE_SEM)==-1){
            perror("releaseSem");
            exit(1);
        }
    }
    if(shmdt(shmp)==-1){
        perror("shmdt");
        exit(1);
    }
    if(releaseSem(semid,WRITE_SEM)==-1){
        perror("releaseSem");
        exit(1);
    }
    fprintf(stderr,"Received %d bytes (%d xfrs)\n",bytes,xfrs);
}