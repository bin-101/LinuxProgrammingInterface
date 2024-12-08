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

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob3$ ./sv
seqNum: 14
^C
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob3$ gcc main_cl.c -ocl
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob3$ ./cl 7
seq_num: 0
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob3$ ./cl 7
seq_num: 7
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob3$ ./cl 8
seq_num: 14
*/

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
    int backupFd=open(BACKUP_FILE,O_RDWR|O_CREAT|O_EXCL|O_SYNC,S_IRUSR|S_IWUSR);
    if(backupFd==-1 && errno!=EEXIST){
        printf("open: %s\n",strerror(errno));
        exit(1);
    }
    int seqNum=0;
    if(backupFd==-1){ // すでにバックアップファイルが存在する
        backupFd=open(BACKUP_FILE,O_RDWR|O_SYNC);
        if(backupFd==-1){
            printf("open: %s\n",strerror(errno));
            exit(1);
        }
        if(read(backupFd,&seqNum,sizeof(seqNum))!=sizeof(seqNum)){
            printf("read: %s\n",strerror(errno));
            exit(1);
        }
        printf("seqNum: %d\n",seqNum);
    }else{
        write(backupFd,&seqNum,sizeof(seqNum)); // 書き込み前に終了してしまうとデータが書き込まれない(本当はシグナルマスクとsigsuspendを使うべき)
    }

    umask(0);

    if(mkfifo(SERVER_FIFO,S_IRUSR|S_IWUSR|S_IWGRP)==-1 && errno!=EEXIST){
        printf("mkfifo: %s\n",strerror(errno));
        exit(1);
    }
    int serverFd=open(SERVER_FIFO,O_RDONLY);
    if(serverFd==-1){
        printf("open: %s\n",strerror(errno));
        exit(1);
    }

    int dummyFd=open(SERVER_FIFO,O_WRONLY);
    if(dummyFd==-1){
        printf("open: %s\n",strerror(errno));
        exit(1);
    }

    if(signal(SIGPIPE,SIG_IGN)==SIG_ERR){
        printf("signal: %s\n",strerror(errno));
        exit(1);
    }

    while(1){
        struct request req;
        if(read(serverFd,&req,sizeof(req))!=sizeof(req)){
            exit(1);
        }
        char clientFifo[CLIENT_FIFO_NAME_LEN];
        snprintf(clientFifo,CLIENT_FIFO_NAME_LEN,CLIENT_FIFO_TEMPLATE,(long)req.pid);
        int clientFd=open(clientFifo,O_WRONLY);
        if(clientFd==-1){
            printf("open: %s\n",strerror(errno));
            exit(1);
        }

        struct response resp;
        resp.seq_num=seqNum;
        if(write(clientFd,&resp,sizeof(resp))!=sizeof(resp)){
            exit(1);
        }
        if(close(clientFd)==-1){
            printf("close: %s\n",strerror(errno));
            exit(1);
        }
        seqNum+=req.seq_len;
        pwrite(backupFd,&seqNum,sizeof(seqNum),0); // オフセットを0にして上書き
    }
}