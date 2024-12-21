#include "example48_1.h"
#include "binary_sems.h"

// 写経

int main(int argc,char *argv[]){
    int semid=semget(SEM_KEY,2,IPC_CREAT|OBJ_PERMS);
    if(semid==-1){
        perror("semget");
        exit(1);
    }
    if(initSemAvailable(semid,WRITE_SEM)==-1){
        perror("initSemAvailable");
        exit(1);
    }
    if(initSemInUse(semid,READ_SEM)==-1){
        perror("initSemInUse");
        exit(1);
    }

    int shmid=shmget(SHM_KEY,sizeof(struct shmseg),IPC_CREAT|OBJ_PERMS);
    if(shmid==-1){
        perror("shmget");
        exit(1);
    }
    struct shmseg *shmp=shmat(shmid,NULL,0);
    if(shmp==(void *)-1){
        perror("shmat");
        exit(1);
    }

    int xfrs;
    int bytes;
    for(xfrs=0,bytes=0;;xfrs++,bytes+=shmp->cnt){
        if(reserveSem(semid,WRITE_SEM)==-1){
            perror("reserveSem");
            exit(1);
        }
        shmp->cnt=read(STDIN_FILENO,shmp->buf,BUF_SIZE);
        if(shmp->cnt==-1){
            perror("read");
            exit(1);
        }
        if(releaseSem(semid,READ_SEM)==-1){
            perror("releaseSem");
            exit(1);
        }
        if(shmp->cnt==0) break;
    }

    if(reserveSem(semid,WRITE_SEM)==-1){
        perror("reserveSem");
        exit(1);
    }
    union semun dummy;
    if(semctl(semid,0,IPC_RMID,dummy)==-1){
        perror("semctl");
        exit(1);
    }
    if(shmdt(shmp)==-1){
        perror("shmdt");
        exit(1);
    }
    if(shmctl(shmid,IPC_RMID,0)==-1){
        perror("shmctl");
        exit(1);
    }

    fprintf(stderr,"Sent %d bytes (%d xfrs)\n",bytes,xfrs);
}