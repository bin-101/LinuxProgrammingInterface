#include "example48_1.h"
#include "../../sec48/prob1/binary_sems.h"

// 最初にsec48/prob1/example48_2.cをコピーした

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob2$ ./writer < remap_file_pages.txt 
Sent 4106 bytes (5 xfrs)

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob2$ ./reader > remap_file_pages_copy.txt
Received 4106 bytes (5 xfrs)

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob2$ diff remap_file_pages.txt remap_file_pages_copy.txt
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob2$ # 一致していることを確認した
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

    int mapping_fd=open(FILE_NAME,O_RDWR|O_CREAT,S_IWUSR|S_IRUSR);
    if(ftruncate(mapping_fd,sizeof(struct shmseg))==-1){
        perror("ftruncate");
        exit(1);
    }
    void *addr=mmap(NULL,sizeof(struct shmseg),PROT_READ|PROT_WRITE,MAP_SHARED,mapping_fd,0);
    if(addr==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    struct shmseg *shmp=(struct shmseg *)addr;

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
    if(msync(addr,sizeof(struct shmseg),MS_SYNC)==-1){
        perror("msync");
        exit(1);
    }

    fprintf(stderr,"Sent %d bytes (%d xfrs)\n",bytes,xfrs);
}