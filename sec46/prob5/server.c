#include "./header.h"

// 例46-8の写経

static void
grimReaper(int sig){
    int savedErrno;
    savedErrno=errno;
    while(waitpid(-1,NULL,WNOHANG)>0){
        continue;
    }
    errno=savedErrno;
}

static void
serveRequest(const struct requestMsg *req){
    int fd=open(req->pathname,O_RDONLY);
    struct responseMsg resp;
    if(fd==-1){
        resp.mtype=RESP_MT_FAILURE;
        snprintf(resp.data,sizeof(resp.data),"%s","couldn't open");
        msgsnd(req->clientId,&resp,strlen(resp.data)+1,0);
        exit(1);
    }

    resp.mtype=RESP_MT_DATA;
    int numRead;
    while((numRead=read(fd,resp.data,RESP_MSG_SIZE))>0){
        if(msgsnd(req->clientId,&resp,numRead,0)==-1){
            break;
        }
    }
    resp.mtype=RESP_MT_END;
    msgsnd(req->clientId,&resp,0,0);
}

int main(int argc,char *argv[]){
    int serverId=msgget(SERVER_KEY,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR|S_IWGRP);

    printf("serverId: %d\n",serverId);
    if(serverId==-1){
        perror("msgget");
        exit(1);
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    sa.sa_handler=grimReaper;
    if(sigaction(SIGCHLD,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }

    for(;;){
        struct requestMsg req;
        int msgLen=msgrcv(serverId,&req,REQ_MSG_SIZE,0,0);
        if(msgLen==-1){
            if(errno=EINTR) continue;
            perror("msgrcv");
            exit(1);
        }

        pid_t pid=fork();
        if(pid==-1){
            perror("fork");
            break;
        }

        if(pid==0){
            serveRequest(&req);
            _exit(0);
        }
    }

    if(msgctl(serverId,IPC_RMID,NULL)==-1){
        perror("msgctl");
    }
}