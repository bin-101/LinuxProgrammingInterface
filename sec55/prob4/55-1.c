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

int main(int argc,char *argv[]){
    int lock=(argv[2][0]=='s')?LOCK_SH:LOCK_EX;
    if(argv[2][1]=='n'){
        lock|=LOCK_NB;
    }

    int fd=open(argv[1],O_RDONLY);
    if(fd==-1){
        perror("open");
        exit(1);
    }

    const char *lname=(lock&LOCK_SH)?"LOCK_SH":"LOCK_EX";
    time_t t=time(NULL);
    printf("PID %ld: requesting %s at %s",(long)getpid(),lname,ctime(&t));

    if(flock(fd,lock)==-1){
        if(errno==EWOULDBLOCK){ // LOCK_NBをセットしており、ロックできない場合
            perror("flock");
            exit(1);
        }else{
            printf("flock (PID=%ld)",(long)getpid());
            exit(1);
        }
    }
    t=time(NULL);
    printf("PID %ld: granted %s at %s",(long)getpid(),lname,ctime(&t));
    sleep((argc>3)?atoi(argv[3]):10);

    t=time(NULL);
    printf("PID %ld: releasing %s at %s",(long)getpid(),lname,ctime(&t));

    if(flock(fd,LOCK_UN)==-1){
        perror("flock");
        exit(1);
    }
}