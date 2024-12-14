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
#include <signal.h>


// 例44-7を参考にする

/*
実行結果
サーバー
~~~~~
seq_num: 73
writed
closed
readed
seq_num: 74
writed
closed

クライアント
~~~~~
seq_num: 73
end
start
serverFd opened
writed
clientFd opened
seq_num: 74
end
start
serverFd opened
sig: 13
write: Broken pipe

サーバーがクローズする前にクライアントがオープンし、サーバーがクローズした後にクライアントが書き込むと、クライアントがSIGPIPEを受け取る。


サーバーの方にusleep(50)を挿入。
サーバー
readed
seq_num: 0
writed
closed

クライアント
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob5$ ./cl
start
serverFd opened
writed
clientFd opened
seq_num: 0
end
start
serverFd opened
writed

サーバーがクローズする前にクライアントがオープンし、サーバーがクローズする前にクライアントが書き込むと、
クライアントが書き込んだ情報が消えており、サーバーがreadできない。
*/

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

void handler(int sig){
    printf("sig: %d\n",sig);
}

int main(int argc,char *argv[]){
    umask(0);

    struct sigaction sa;
    sa.sa_handler=handler;
    sa.sa_flags=SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGPIPE,&sa,NULL)==-1){
        printf("sigaction: %s\n",strerror(errno));
        exit(1);
    }



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
    req.seq_len=1;

    // int clientFd=open(clientFifo,O_RDONLY);
    // if(clientFd==-1){
    //     printf("open: %s\n",strerror(errno));
    //     exit(1);
    // }
    // fcntl(clientFd,F_SETFL,fcntl(clientFd,F_GETFL)&~O_NONBLOCK);

    while(1){
        printf("start\n");
        int serverFd=open(SERVER_FIFO,O_WRONLY);
        printf("serverFd opened\n");
        if(serverFd==-1){
            printf("open: %s\n",strerror(errno));
            exit(1);
        }
        if(write(serverFd,&req,sizeof(req))!=sizeof(req)){
            // if(errno==EPIPE){
            //     printf("write: Broken pipe\n");
            // }
            printf("write: %s\n",strerror(errno));
            exit(1);
        }
        printf("writed\n");

        if(close(serverFd)==-1){
            printf("close: %s\n",strerror(errno));
            exit(1);
        }

        int clientFd=open(clientFifo,O_RDONLY);
        if(clientFd==-1){
            printf("open: %s\n",strerror(errno));
            exit(1);
        }
        printf("clientFd opened\n");

        struct response resp;
        int x;
        if((x=read(clientFd,&resp,sizeof(resp)))!=sizeof(resp)){
            // メモ: close(serverFd)を実行しない場合、相手がcloseする前にopenし、相手のwriteが終わる前にreadするとEOFになった。
            printf("x: %d sizeof(resp): %ld\n",x,sizeof(resp));
            printf("resp: %d\n",resp.seq_num);
            printf("read: %s\n",strerror(errno));
            exit(1);
        }
        printf("seq_num: %d\n",resp.seq_num);

        if(close(clientFd)==-1){
            printf("close: %s\n",strerror(errno));
            exit(1);
        }

        printf("end\n");
    }
}
