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

/*
実行結果
サーバー側
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob2$ gcc sv.c -osv
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob2$ ./sv
pid: 131873
^C
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob2$ echo $?
130

クライアント側
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob2$ gcc cl.c -ocl
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob2$ ./cl 10
pid: 131932
seq_num: 0
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob2$ ./cl 1
pid: 131953
seq_num: 10
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec52/prob2$ ./cl 1
pid: 131977
seq_num: 11

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ ls  /dev/mqueue/
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ # キューが削除できていることを確認
*/

#define SERVER_MQ "/mq52-2"
#define CLIENT_MQ_TEMPLATE "/mq52-2.%ld"
#define CLIENT_MQ_NAME_LEN (sizeof(CLIENT_MQ_TEMPLATE) + 20)

struct request {
    pid_t pid;
    int seq_len;
};

struct response {
    int seq_num;
};

void removeMQ(int sig){
    if(mq_unlink(SERVER_MQ)==-1){
        perror("mq_unlink");
    }
    raise(SIGINT);
}

int main(int argc,char *argv[]){
    int seqNum=0;
    umask(0);
    printf("pid: %d\n",getpid());
    struct sigaction sa;
    sa.sa_handler=removeMQ;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESETHAND;
    if(sigaction(SIGINT,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }

    int mqd=mq_open(SERVER_MQ,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR,NULL);
    if(mqd==(mqd_t)-1){
        perror("mq_open");
        exit(1);
    }

    while(1){
        struct mq_attr attr;
        if(mq_getattr(mqd,&attr)==-1){
            perror("mq_getattr");
            exit(1);
        }
        struct request req;
        char buffer[attr.mq_msgsize];
        int numRead=mq_receive(mqd,buffer,attr.mq_msgsize,NULL);
        if(numRead==-1){
            perror("mq_receive");
            exit(1);
        }
        memcpy(&req, buffer, sizeof(req));

        char clientMQ[CLIENT_MQ_NAME_LEN];
        snprintf(clientMQ,CLIENT_MQ_NAME_LEN,CLIENT_MQ_TEMPLATE,(long)req.pid);
        int clientMQd=mq_open(clientMQ,O_WRONLY);
        if(clientMQd==-1){
            perror("mq_open");
            exit(1);
        }

        struct response resp;
        resp.seq_num=seqNum;
        if(mq_send(clientMQd,(char *)&resp,sizeof(resp),0)==-1){
            perror("mq_send");
            exit(1);
        }
        if(mq_close(clientMQd)==-1){
            perror("mq_close");
            exit(1);
        }
        seqNum+=req.seq_len;
    }
}