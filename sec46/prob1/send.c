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
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

// 例46-2 svmsg_send.c の写経

#define MAX_MTEXT 1024

struct mbuf{
    long mtype;
    char mtext[MAX_MTEXT];
};

int main(int argc, char *argv[]){

    int flags=0;
    int opt;
    while((opt=getopt(argc,argv,"n"))!=-1){
        if(opt=='n'){
            flags|=IPC_NOWAIT;
        }else{
            perror("getopt");
        }
    }

    struct mbuf msg;
    int msqid=atoi(argv[optind]);
    msg.mtype=atoi(argv[optind+1]);
    int msgLen=0;
    if(argc>optind+2){
        msgLen=strlen(argv[optind+2])+1;
        if(msgLen>MAX_MTEXT){
            exit(1);
        }
        memcpy(msg.mtext,argv[optind+2],msgLen);
    }

    if(msgsnd(msqid,&msg,msgLen,flags)==-1){
        perror("msgsnd\n");
    }
    exit(0);
}