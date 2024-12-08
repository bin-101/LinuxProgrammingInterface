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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob4$ gcc main.c
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob4$ ./a.out
^Csignal: 2

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob4$ ls /tmp/seqnum_sv
ls: '/tmp/seqnum_sv' にアクセスできません: そのようなファイルやディレクトリはありません
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob4$ ./a.out
signal: 15 # killall a.out
Terminated
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob4$ ls /tmp/seqnum_sv
ls: '/tmp/seqnum_sv' にアクセスできません: そのようなファイルやディレクトリはありません
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob4$ ./a.out
強制終了 # killall -SIGKILL a.out
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob4$ ls /tmp/seqnum_sv
/tmp/seqnum_sv

終了ステータスを確認
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob4$ ./a.out
^Csignal: 2

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob4$ echo $?
130 # 128+2(SIGINT)
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

void removeFifo(int sig){
    printf("signal: %d\n",sig);
    unlink(SERVER_FIFO);
    signal(sig,SIG_DFL); // 26.1.4 を参考にした。
    raise(sig); 
}

int main(int argc,char *argv[]){
    int seqNum=0;

    umask(0);

    if(mkfifo(SERVER_FIFO,S_IRUSR|S_IWUSR|S_IWGRP)==-1 && errno!=EEXIST){
        printf("mkfifo: %s\n",strerror(errno));
        exit(1);
    }

    struct sigaction sa;
    sa.sa_handler=removeFifo;
    sa.sa_flags=0;
    sigfillset(&sa.sa_mask);
    if(sigaction(SIGINT,&sa,NULL)==-1){
        printf("sigaction: %s\n",strerror(errno));
        exit(1);
    }
    if(sigaction(SIGTERM,&sa,NULL)==-1){
        printf("sigaction: %s\n",strerror(errno));
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
    }
}