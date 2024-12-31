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

int main(){
    int fd=open("a.txt",O_RDWR);
    if(fd==-1){
        perror("open");
        exit(1);
    }
    if(ftruncate(fd,80000)==-1){
        perror("ftruncate");
        exit(1);
    }

    struct flock fl;
    fl.l_type=F_WRLCK;
    fl.l_whence=SEEK_SET;
    fl.l_len=1;

    for(int i=0;i<=40000;i++){
        fl.l_start=i*2;
        if(fcntl(fd,F_SETLK,&fl)==-1){
            perror("fcntl");
            exit(1);
        }
    }
    pause();
}