#include "header.h"
#include "../../sec48/prob1/binary_sems.h"

int main(int argc,char *argv[]){
    int semid=semget(SEM_KEY,0,0);
    if(semid==-1){
        perror("semget");
        exit(1);
    }

    int shm_fd=shm_open(SHM_NAME,O_RDONLY,0);
    if(shm_fd==-1){
        perror("shm_open");
        exit(1);
    }
    struct shmseg *shmp=mmap(NULL,sizeof(struct shmseg),PROT_READ,MAP_SHARED,shm_fd,0);
    if(shmp==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    if(close(shm_fd)==-1){
        perror("close");
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
    if(releaseSem(semid,WRITE_SEM)==-1){
        perror("releaseSem");
        exit(1);
    }
    fprintf(stderr,"Received %d bytes (%d xfrs)\n",bytes,xfrs);
}