#include "header.h"
#include "../../sec48/prob1/binary_sems.h"

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec54/prob1$ ./writer
fa
faffafaf
Sent 12 bytes (2 xfrs)
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec54/prob1$ ./reader
fa
faffafaf
Received 12 bytes (2 xfrs)
*/

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

    int shm_fd=shm_open(SHM_NAME,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
    if(shm_fd==-1){
        perror("shm_open");
        exit(1);
    }
    if(ftruncate(shm_fd,sizeof(struct shmseg))==-1){
        perror("ftruncate");
        exit(1);
    }

    int xfrs;
    int bytes;
    struct shmseg *shmp=mmap(NULL,sizeof(struct shmseg),PROT_READ|PROT_WRITE,MAP_SHARED,shm_fd,0);
    if(shmp==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    if(close(shm_fd)==-1){
        perror("close");
        exit(1);
    }

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
    if(shm_unlink(SHM_NAME)==-1){
        perror("shm_unlink");
        exit(1);
    }

    fprintf(stderr,"Sent %d bytes (%d xfrs)\n",bytes,xfrs);
}