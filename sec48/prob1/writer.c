#include "example48_1.h"
#include "eventflag.h"

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob1$ gcc writer.c -owriter
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob1$ ./writer
abcde
iuiuiu
Sent 13 bytes (2 xfrs)

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob1$ gcc reader.c -oreader
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob1$ ./reader
abcde
iuiuiu
Received 13 bytes (2 xfrs)

セマフォと共有メモリが削除されていることを確認する
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob1$ ./reader
semget: No such file or directory

int shmid=shmget(SHM_KEY,sizeof(struct shmseg),IPC_CREAT|OBJ_PERMS);のIPC_CREATを消して以下を実行
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob1$ gcc writer.c -owriter
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob1$ ./writer
shmget: No such file or directory
*/

int main(int argc,char *argv[]){
    int semid=semget(SEM_KEY,2,IPC_CREAT|OBJ_PERMS);
    if(semid==-1){
        perror("semget");
        exit(1);
    }
    if(setEventFlag(semid,WRITE_SEM)==-1){
        perror("setEventFlag");
        exit(1);
    }
    if(clearEventFlag(semid,READ_SEM)==-1){
        perror("clearEventFlag");
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
        if(waitForEventFlag(semid,WRITE_SEM)==-1){
            perror("waitForEventFlag");
            exit(1);
        }
        // waitとclearがatomicではないのが気になるが、今回のプログラムでは問題にならない
        if(clearEventFlag(semid,WRITE_SEM)==-1){
            perror("clearEventFlag");
            exit(1);
        }
        shmp->cnt=read(STDIN_FILENO,shmp->buf,BUF_SIZE);
        if(shmp->cnt==-1){
            perror("read");
            exit(1);
        }
        if(setEventFlag(semid,READ_SEM)==-1){
            perror("setEventFlag");
            exit(1);
        }
        if(shmp->cnt==0) break;
    }

    if(waitForEventFlag(semid,WRITE_SEM)==-1){
        perror("waitForEventFlag");
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