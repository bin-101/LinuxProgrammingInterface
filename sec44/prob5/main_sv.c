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
#include <sys/stat.h>

// 例44-7を参考にする

#define SERVER_FIFO "/tmp/seqnum_sv"
#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld"
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)
#define BACKUP_FILE "./backup.txt"

struct request {
    pid_t pid;
    int seq_len;
};

struct response {
    int seq_num;
};

int main(int argc,char *argv[]){
    int seqNum=0;
    umask(0);

    if(mkfifo(SERVER_FIFO,S_IRUSR|S_IWUSR|S_IWGRP)==-1 && errno!=EEXIST){
        printf("mkfifo: %s\n",strerror(errno));
        exit(1);
    }
    int serverFd=open(SERVER_FIFO,O_RDONLY);
    if(serverFd==-1){
        printf("open0: %s\n",strerror(errno));
        exit(1);
    }

    // int dummyFd=open(SERVER_FIFO,O_WRONLY);
    // if(dummyFd==-1){
    //     printf("open: %s\n",strerror(errno));
    //     exit(1);
    // }

    if(signal(SIGPIPE,SIG_IGN)==SIG_ERR){
        printf("signal: %s\n",strerror(errno));
        exit(1);
    }

    while(1){
        struct request req;
        int numRead;
        while((numRead=read(serverFd,&req,sizeof(req)))!=sizeof(req)){
            if(numRead!=0){
                printf("read: %s\n",strerror(errno));
                exit(1);
            }
            usleep(50);
            close(serverFd);
            serverFd=open(SERVER_FIFO,O_RDONLY); // openし直す。クライアントがオープンするまでブロックする
        }
        printf("readed\n");
        char clientFifo[CLIENT_FIFO_NAME_LEN];
        snprintf(clientFifo,CLIENT_FIFO_NAME_LEN,CLIENT_FIFO_TEMPLATE,(long)req.pid);
        int clientFd=open(clientFifo,O_WRONLY);
        if(clientFd==-1){
            printf("open1: %s\n",strerror(errno));
            exit(1);
        }

        struct response resp;
        resp.seq_num=seqNum;
        printf("seq_num: %d\n",resp.seq_num);
        int x=0;
        if((x=write(clientFd,&resp,sizeof(resp)))!=sizeof(resp)){
            printf("x: %d sizeof(resp): %ld\n",x,sizeof(resp));
            printf("write: %s\n",strerror(errno));
            exit(1);
        }
        printf("writed\n");
        if(close(clientFd)==-1){
            printf("close: %s\n",strerror(errno));
            exit(1);
        }
        printf("closed\n");
        seqNum+=req.seq_len;
    }
}
