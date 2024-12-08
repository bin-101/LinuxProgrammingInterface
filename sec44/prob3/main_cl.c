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

struct request {
    pid_t pid;
    int seq_len;
};

struct response {
    int seq_num;
};

char clientFifo[CLIENT_FIFO_NAME_LEN];

void removeFile(){
    unlink(clientFifo);
}

int main(int argc,char *argv[]){
    umask(0);

    snprintf(clientFifo,CLIENT_FIFO_NAME_LEN,CLIENT_FIFO_TEMPLATE,(long)getpid());
    if(mkfifo(clientFifo,S_IRUSR|S_IWUSR|S_IWGRP)==-1 && errno!=EEXIST){
        printf("mkfifo: %s\n",strerror(errno));
        exit(1);
    }

    if(atexit(removeFile)!=0){
        printf("atexit: %s\n",strerror(errno));
        exit(1);
    }

    struct request req;
    req.pid=getpid();
    req.seq_len=atoi(argv[1]);

    int serverFd=open(SERVER_FIFO,O_WRONLY);
    if(serverFd==-1){
        printf("open: %s\n",strerror(errno));
        exit(1);
    }

    if(write(serverFd,&req,sizeof(req))!=sizeof(req)){
        printf("write: %s\n",strerror(errno));
        exit(1);
    }

    int clientFd=open(clientFifo,O_RDONLY);
    if(clientFd==-1){
        printf("open: %s\n",strerror(errno));
        exit(1);
    }

    struct response resp;
    if(read(clientFd,&resp,sizeof(resp))!=sizeof(resp)){
        exit(1);
    }

    printf("seq_num: %d\n",resp.seq_num);
}