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

#define MAX_MTEXT 2

struct mbuf{
    long mtype;
    char mtext[MAX_MTEXT];
};

int main(int argc,char *argv[]){


    int opt;
    int flags=0;
    int type=0;
    while((opt=getopt(argc,argv,"ent:x"))!=-1){
        switch(opt){
            case 'e':
                flags|=MSG_NOERROR; break;
            case 'n':
                flags|=IPC_NOWAIT; break;
            case 't':
                type=atoi(optarg); break;
            case 'x':
                flags|=MSG_EXCEPT; break;
            default:
                printf("default\n");
                exit(1); 
        }
    }

    int msqid=atoi(argv[optind]);
    int maxBytes=(argc>optind+1)?atoi(argv[optind+1]):MAX_MTEXT;
    struct mbuf msg;
    int msgLen=msgrcv(msqid,&msg,maxBytes,type,flags);
    printf("Received: type=%ld; length=%ld",msg.mtype,(long)msgLen);
    if(msgLen>0){
        printf("; body=%s",msg.mtext);
    }
    printf("\n");
}