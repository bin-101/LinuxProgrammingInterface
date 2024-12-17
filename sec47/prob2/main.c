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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob2$ ./a.out
semid: 20
[Fri Jul  9 17:06:23 1982
 74103] Parent about to wait for semaphore to be 0
[Tue Dec 17 09:13:32 2024
 74104] Child started
[Tue Dec 17 09:13:32 2024
 74104] Child about to set semaphore to 0
[Fri Jul  9 17:06:23 1982
 74103] Parent found that semaphore is 0
*/

// 最初に例24-6を写経した

#define SYNC_SIG SIGUSR1

static void
handler(int sig){

}

int
main_24_6(int argc,char *argv){
    setbuf(stdout,NULL);

    sigset_t blockMask,origMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask,SYNC_SIG);
    if(sigprocmask(SIG_BLOCK,&blockMask,&origMask)==-1){
        perror("sigprocmask");
        exit(1);
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    sa.sa_handler=handler;
    if(sigaction(SYNC_SIG,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }

    pid_t childPid;
    switch(childPid=fork()){
        case -1:
            perror("fork");
            exit(1);
        case 0:
            time_t t=time(NULL);
            printf("[%s %d] Child started\n", ctime(&t), getpid());
            sleep(2);
            printf("[%s %d] Child about to signal parent\n", ctime(&t), getpid());
            if(kill(getppid(),SYNC_SIG)==-1){
                perror("kill");
                _exit(1);
            }
            _exit(0);
        default:
            printf("[%s %d] Parent about to wait for signal\n", ctime(&t), getpid());
            sigset_t emptyMask;
            sigemptyset(&emptyMask);
            if(sigsuspend(&emptyMask)==-1 && errno!=EINTR){
                perror("sigsuspend");
                exit(1);
            }
            printf("[%s %d] Parent got signal\n", ctime(&t), getpid());

            if(sigprocmask(SIG_SETMASK,&origMask,NULL)==-1){
                perror("sigprocmask");
                exit(1);
            }

            exit(0);
    }
}

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main(){
    // セマフォ作成
    int semid=semget(IPC_PRIVATE,1,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
    if(semid==-1){
        perror("semget");
        exit(1);
    }
    union semun arg;
    arg.val=1;
    if(semctl(semid,0,SETVAL,arg)==-1){
        perror("semctl");
        exit(1);
    }

    printf("semid: %d\n",semid);

    pid_t childPid;
    switch(childPid=fork()){
        case -1:
            perror("fork");
            exit(1);
        case 0:
            time_t t=time(NULL);
            printf("[%s %d] Child started\n", ctime(&t), getpid());
            sleep(2);
            printf("[%s %d] Child about to set semaphore to 0\n", ctime(&t), getpid());
            union semun arg;
            arg.val=0;
            if(semctl(semid,0,SETVAL,arg)==-1){
                perror("semctl");
                exit(1);
            }
            _exit(0);
        default:
            printf("[%s %d] Parent about to wait for semaphore to be 0\n", ctime(&t), getpid());
            struct sembuf sops;
            sops.sem_num=0;
            sops.sem_op=0;
            sops.sem_flg=0;
            if(semop(semid,&sops,1)==-1){
                perror("semop");
                exit(1);
            }
            printf("[%s %d] Parent found that semaphore is 0\n", ctime(&t), getpid());

            exit(0);
    }
}