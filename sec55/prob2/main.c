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
#include <sys/mman.h>
#include <sys/file.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob2$ time ./a.out
^C

real    0m11.394s
user    0m0.002s
sys     0m0.002s
flockはデッドロックを検出できないことが分かった
*/

void child_process(){
    int fd_a=open("a.txt",O_RDONLY);
    if(fd_a==-1){
        perror("open");
        exit(1);
    }
    int fd_b=open("b.txt",O_RDONLY);
    if(fd_b==-1){
        perror("open");
        exit(1);
    }

    if(flock(fd_b,LOCK_EX)==-1){
        perror("flock");
        exit(1);
    }
    sleep(1); // 親プロセスのflock(fd_a,LOCK_EX)を先に行う
    if(flock(fd_a,LOCK_EX)==-1){
        perror("flock");
        exit(1);
    }

    printf("child finished!\n");
}

int main(int argc,char *argv[]){
    if(fork()==0){
        child_process();
        _exit(0);
    }

    int fd_a=open("a.txt",O_RDONLY);
    if(fd_a==-1){
        perror("open");
        exit(1);
    }
    int fd_b=open("b.txt",O_RDONLY);
    if(fd_b==-1){
        perror("open");
        exit(1);
    }

    if(flock(fd_a,LOCK_EX)==-1){
        perror("flock");
        exit(1);
    }
    sleep(2);
    if(flock(fd_b,LOCK_EX)==-1){
        perror("flock");
        exit(1);
    }
    printf("parent finished!\n");
}