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
#include <sys/msg.h>

// 44-3の解答をコピペして編集する

struct request {
    long mtype;  // 1 サーバーはmtype1のメッセージを受け取る
    int pid;
    int num;
};

struct response {
    long mtype; // クライアントのプロセスID
    int num;
};

int main(int argc,char *argv[]){
    // メッセージキューオープン
    int msqId=msgget(ftok("./test.txt",1),0);
    printf("msqId: %d\n",msqId);
    if(msqId==-1){
        printf("msgget: %s\n",strerror(errno));
        exit(1);
    }

    struct request req;
    req.mtype=1;
    req.pid=getpid();
    req.num=atoi(argv[1]);
    printf("getpid(): %d\n",getpid());

    if(msgsnd(msqId,&req,8,0)==-1){
        perror("msqId");
    }

    struct response resp;
    if(msgrcv(msqId,&resp,4,getpid(),0)==-1){
        perror("msgrcv");
    }

    printf("seq_num: %d\n",resp.num);
}