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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob3$ ls -i a.txt
27155490 a.txt # i-nodeを確認

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob3$ ./a.out
first:


flock(fd,LOCK_EX):
36628: FLOCK  ADVISORY  WRITE 156843 103:07:27155490 0 EOF


flock(new_fd,LOCK_UN): # dupで複製したファイルディスクリプタでUNLOCKした場合でも、UNLOCKされることを確認


flock(fd,LOCK_EX):
36628: FLOCK  ADVISORY  WRITE 156843 103:07:27155490 0 EOF


close(fd):
36628: FLOCK  ADVISORY  WRITE 156843 103:07:27155490 0 EOF


close(new_fd): # 複製も含めたファイルディスクリプタすべてをcloseすると、UNLOCKされることを確認


flock(fd,LOCK_EX):
36635: FLOCK  ADVISORY  WRITE 156843 103:07:27155490 0 EOF


fork()->flock(fd,LOCK_UN): # 子プロセスが受け継いだファイルディスクリプタでUNLOCKした場合でも、UNLOCKされることを確認


*/

void print(char *str){
    printf("%s\n",str);
    system("cat /proc/locks | grep 27155490");
    printf("\n\n");
}

int main(int argc,char *argv[]){
    int fd=open("a.txt",O_RDONLY);
    if(fd==-1){
        perror("open");
        exit(1);
    }
    print("first:");
    if(flock(fd,LOCK_EX)==-1){
        perror("flock");
        exit(1);
    }
    print("flock(fd,LOCK_EX):");

    int new_fd=dup(fd);
    if(new_fd==-1){
        perror("dup");
        exit(1);
    }
    if(flock(new_fd,LOCK_UN)==-1){
        perror("flock");
        exit(1);
    }
    print("flock(new_fd,LOCK_UN):");

    if(flock(fd,LOCK_EX)==-1){
        perror("flock");
        exit(1);
    }
    print("flock(fd,LOCK_EX):");

    if(close(fd)==-1){
        perror("close");
        exit(1);
    }
    print("close(fd):");

    if(close(new_fd)==-1){
        perror("close");
        exit(1);
    }
    print("close(new_fd):");

    fd=open("a.txt",O_RDONLY);
    if(fd==-1){
        perror("open");
        exit(1);
    }
    if(flock(fd,LOCK_EX)==-1){
        perror("flock");
        exit(1);
    }
    print("flock(fd,LOCK_EX):");

    if(fork()==0){
        if(flock(fd,LOCK_UN)==-1){
            perror("flock");
            exit(1);
        }
        _exit(0);
    }
    if(wait(NULL)==-1){
        perror("wait");
        exit(1);
    }
    print("fork()->flock(fd,LOCK_UN):");
}