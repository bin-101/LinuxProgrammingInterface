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
#include <limits.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob4$ ./a.out
shmid: 262200
pid: 97976
detached

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob4$ ./a.out 262200
not destroyed
locked
Last changed: Sun Dec 22 18:39:02 2024
Last shmat(): Sun Dec 22 18:39:02 2024
Last shmdt(): Thu Jan  1 09:00:00 1970
Owner pid: 97976
Last shmat()/shmdt() pid: 97976
number of attach: 1
size of segment in bytes: 1
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob4$ ./a.out 262200 # detached が出力された後実行
not destroyed
locked
Last changed: Sun Dec 22 18:39:02 2024
Last shmat(): Sun Dec 22 18:39:02 2024
Last shmdt(): Sun Dec 22 18:39:32 2024 # 更新されている
Owner pid: 97976
Last shmat()/shmdt() pid: 97976
number of attach: 0 # 0になっている
size of segment in bytes: 1
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec48/prob4$ ./a.out 262200 # ./a.outが終了してから実行
shmctl: Invalid argument

*/

int main(int argc,char *argv[]){
    if(argc<=1){
        int shmid=shmget(IPC_PRIVATE,1,IPC_CREAT|S_IRUSR|S_IWUSR);
        printf("shmid: %d\n",shmid);
        printf("pid: %d\n",getpid());
        void *addr=shmat(shmid,NULL,SHM_RDONLY);
        if(addr==(void *)-1){
            perror("shmat");
            exit(1);
        }
        shmctl(shmid,SHM_LOCK,NULL);
        sleep(30);
        if(shmdt(addr)==-1){
            perror("shmdt");
            exit(1);
        }
        printf("detached\n");
        sleep(20);
        if(shmctl(shmid,IPC_RMID,NULL)==-1){
            perror("shmctl");
            exit(1);
        }
        exit(0);
    }
    int shmid=atoi(argv[1]);

    struct shmid_ds buf;
    if(shmctl(shmid,IPC_STAT,&buf)==-1){
        perror("shmctl");
        exit(1);
    }
    if(buf.shm_perm.mode&SHM_DEST){
        printf("destroyed\n");
    }else{
        printf("not destroyed\n");
    }
    if(buf.shm_perm.mode&SHM_LOCKED){
        printf("locked\n");
    }else{
        printf("not locked\n");
    }

    printf("Last changed: %s",ctime(&buf.shm_ctime));
    printf("Last shmat(): %s",ctime(&buf.shm_atime));
    printf("Last shmdt(): %s",ctime(&buf.shm_dtime));
    printf("Owner pid: %d\n",buf.shm_cpid);
    printf("Last shmat()/shmdt() pid: %d\n",buf.shm_lpid);
    printf("number of attach: %ld\n",buf.shm_nattch);
    printf("size of segment in bytes: %ld\n",buf.shm_segsz);
}