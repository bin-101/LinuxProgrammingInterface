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

char clientMQ[CLIENT_MQ_NAME_LEN];

void removeMQ(){
    if(mq_unlink(clientMQ)==-1){
        perror("mq_unlink");
    }
}


int main(int argc,char *argv[]){
    umask(0);
    printf("pid: %d\n",getpid());

    snprintf(clientMQ,CLIENT_MQ_NAME_LEN,CLIENT_MQ_TEMPLATE,(long)getpid());
    int clientMQd=mq_open(clientMQ,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR,NULL);
    if(clientMQd==-1){
        perror("mq_open");
        exit(1);
    }
    if(atexit(removeMQ)!=0){
        printf("atexit: %s\n",strerror(errno));
        exit(1);
    }

    struct request req;
    req.pid=getpid();
    req.seq_len=atoi(argv[1]);

    int serverMQd=mq_open(SERVER_MQ,O_WRONLY);
    if(serverMQd==-1){
        perror("mq_open");
        exit(1);
    }
    if(mq_send(serverMQd,(char *)&req,sizeof(req),0)==-1){
        perror("mq_send");
        exit(1);
    }

    struct mq_attr attr;
    if(mq_getattr(clientMQd,&attr)==-1){
        perror("mq_getattr");
        exit(1);
    }

    char buffer[attr.mq_msgsize];
    int numRead=mq_receive(clientMQd,buffer,attr.mq_msgsize,NULL);
    if(numRead==-1){
        perror("mq_receive");
        exit(1);
    }

    struct response resp;
    memcpy(&resp, buffer, sizeof(resp));
    printf("seq_num: %d\n",resp.seq_num);
}