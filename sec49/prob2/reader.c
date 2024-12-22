#include "example48_1.h"
#include "../../sec48/prob1/binary_sems.h"

// 最初にsec48/prob1/example48_3.cをコピーした

int main(int argc,char *argv[]){
    int semid=semget(SEM_KEY,0,0);
    if(semid==-1){
        perror("semget");
        exit(1);
    }

    int mapping_fd=open(FILE_NAME,O_RDONLY);
    void *addr=mmap(NULL,sizeof(struct shmseg),PROT_READ,MAP_SHARED,mapping_fd,0);
    if(addr==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    struct shmseg *shmp=(struct shmseg *)addr;

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