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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob8$ ./a.out
EDEADLK: 35
process B(fd_a): : Resource deadlock avoided
errno: 35
process C finished!
process A finished!
プロセス3つによるデッドロックを検知することを確かめられた。
*/


int fd_a,fd_b,fd_c;

void open_abc(){
    fd_a=open("a.txt",O_RDWR); // ライトロックをする場合は書き込み可能な必要がある
    if(fd_a==-1){
        perror("open");
        exit(1);
    }
    fd_b=open("b.txt",O_RDWR);
    if(fd_b==-1){
        perror("open");
        exit(1);
    }
    fd_c=open("c.txt",O_RDWR);
    if(fd_c==-1){
        perror("open");
        exit(1);
    }
}

int main(int argc,char *argv[]){
    struct flock fl;
    fl.l_type=F_WRLCK;
    fl.l_whence=SEEK_SET;
    fl.l_start=0;
    fl.l_len=0; // 0の場合。ファイル末尾までを意味する
    printf("EDEADLK: %d\n",EDEADLK);

    if(fork()==0){
        if(fork()==0){
            // プロセスC
            open_abc();
            if(fcntl(fd_c,F_SETLKW,&fl)==-1){
                perror("process C(fd_c): ");
                exit(1);
            }
            sleep(1);
            if(fcntl(fd_b,F_SETLKW,&fl)==-1){
                perror("process C(fd_b): ");
                printf("errno: %d\n",errno);
                exit(1);
            }
            printf("process C finished!\n");
            _exit(0);
        }
        // プロセスB
        open_abc();
        if(fcntl(fd_b,F_SETLKW,&fl)==-1){
            perror("process B(fd_b): ");
            exit(1);
        }
        sleep(1);
        if(fcntl(fd_a,F_SETLKW,&fl)==-1){
            perror("process B(fd_a): ");
            printf("errno: %d\n",errno);
            exit(1);
        }
        printf("process B finished!\n");
        _exit(0);
    }

    open_abc();
    if(fcntl(fd_a,F_SETLKW,&fl)==-1){
        perror("process A(fd_a): ");
        exit(1);
    }
    sleep(1);
    if(fcntl(fd_c,F_SETLKW,&fl)==-1){
        perror("process A(fd_c): ");
        printf("errno: %d\n",errno);
        exit(1);
    }
    printf("process A finished!\n");
}