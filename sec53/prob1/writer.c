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
#include <sys/resource.h>
#include <mqueue.h>
#include <semaphore.h>

/*
例48-1,48-2,48-3,53-6を参考に実装する

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec53/prob1$ ./a.out
rfafaf
rfafaf
fafaff
fafaff # ^Dを押す
Sent 14 bytes (2 xfrs)
Received 14 bytes (2 xfrs)
*/

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

struct shmseg{
    int cnt;
    char buf[BUF_SIZE];
};

sem_t writer_sem,reader_sem;
struct shmseg shm;

void *reader(void *){
    int xfrs;
    int bytes;
    for(xfrs=0,bytes=0;;xfrs++){
        if(sem_wait(&reader_sem)==-1){
            perror("sem_wait");
            exit(1);
        }
        if(shm.cnt==0){
            break;
        }
        bytes+=shm.cnt;
        if(write(STDOUT_FILENO,shm.buf,shm.cnt)!=shm.cnt){
            perror("partial write");
            exit(1);
        }
        if(sem_post(&writer_sem)==-1){
            perror("sem_post");
            exit(1);
        }
    }
    if(sem_post(&writer_sem)==-1){
        perror("sem_post");
        exit(1);
    }
    fprintf(stderr,"Received %d bytes (%d xfrs)\n",bytes,xfrs);
}

void *writer(void *){
    int xfrs;
    int bytes;
    for(xfrs=0,bytes=0;;xfrs++,bytes+=shm.cnt){
        if(sem_wait(&writer_sem)==-1){
            perror("sem_wait");
            exit(1);
        }

        shm.cnt=read(STDIN_FILENO,shm.buf,BUF_SIZE);
        if(shm.cnt==-1){
            perror("read");
            exit(1);
        }
        if(sem_post(&reader_sem)==-1){
            perror("sem_post");
            exit(1);
        }
        if(shm.cnt==0) break;
    }
    if(sem_wait(&writer_sem)==-1){
        perror("sem_wait");
        exit(1);
    }

    fprintf(stderr,"Sent %d bytes (%d xfrs)\n",bytes,xfrs);
}

int main(int argc,char *argv[]){
    pthread_t writer_thread,reader_thread;
    if(sem_init(&writer_sem,0,1)==-1){ // psharedが0の場合、プロセスと同じライフタイムになる(つまり、明示的に削除する必要なし)
        perror("sem_init");
        exit(1);
    }
    if(sem_init(&reader_sem,0,0)==-1){
        perror("sem_init");
        exit(1);
    }

    if(pthread_create(&writer_thread,NULL,writer,NULL)!=0){
        perror("pthread_create");
        exit(1);
    }
    if(pthread_create(&reader_thread,NULL,reader,NULL)!=0){
        perror("pthread_create");
        exit(1);
    }
    if(pthread_join(writer_thread,NULL)!=0){
        perror("pthread_join");
        exit(1);
    }
    if(pthread_join(reader_thread,NULL)!=0){
        perror("pthread_join");
        exit(1);
    }
}