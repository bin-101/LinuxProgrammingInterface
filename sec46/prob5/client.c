#include "./header.h"

// 例46-9の写経

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob5$ ./client ./sleep.txt
Received 1404 bytes (2 messages)

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob5$ ./client ./sleep.txt c
sig: 14
msgrcv: Interrupted system call

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob5$ ./client ./sleep.txt s
sig: 14
msgsnd: Interrupted system call
*/

static int clientId;

static void
removeQueue(void){
    if(msgctl(clientId,IPC_RMID,NULL)==-1){
        perror("msgctl");
        exit(1);
    }
}

static void
sigHandler(int sig){
    printf("sig: %d\n",sig);
}

static void
changeQbytes(int msq_id){ // メッセージキューの容量を変更
    struct msqid_ds buf;
    if(msgctl(msq_id,IPC_STAT,&buf)==-1){
        exit(1);
    }
    buf.msg_qbytes=1;
    if(msgctl(msq_id,IPC_SET,&buf)==-1){
        exit(1);
    }
}

int main(int argc,char *argv[]){
    int serverId=msgget(SERVER_KEY,S_IWUSR);
    if(serverId==-1){
        perror("msgget");
        exit(1);
    }

    clientId=msgget(IPC_PRIVATE,S_IRUSR|S_IWUSR|S_IWGRP);
    if(clientId==-1){
        perror("msgget");
        exit(1);
    }

    if(argc>=3){
        if(argv[2][0]=='s'){
            changeQbytes(serverId);
        }else{
            changeQbytes(clientId);
        }
    }

    if(atexit(removeQueue)!=0){
        perror("atexit");
        exit(1);
    }

    struct requestMsg req;
    req.mtype=1;
    req.clientId=clientId;
    strncpy(req.pathname,argv[1],sizeof(req.pathname)-1);
    req.pathname[sizeof(req.pathname)-1]='\0';

    // SIGALRMのシグナルハンドラを設定 p512を参考にした
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler=sigHandler;
    if(sigaction(SIGALRM,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }

    alarm(2);
    if(msgsnd(serverId,&req,REQ_MSG_SIZE,0)==-1){
        perror("msgsnd");
        exit(1);
    }
    alarm(0); // 無効化

    alarm(2);
    struct responseMsg resp;
    int msgLen=msgrcv(clientId,&resp,RESP_MSG_SIZE,0,0);
    if(msgLen==-1){
        perror("msgrcv");
        exit(1);
    }
    alarm(0);

    if(resp.mtype==RESP_MT_FAILURE){
        printf("%s\n",resp.data);
        exit(1);
    }

    int totBytes=msgLen;
    int numMsgs;
    alarm(2);
    for(numMsgs=1;resp.mtype==RESP_MT_DATA;numMsgs++){
        alarm(0);
        int msgLen=msgrcv(clientId,&resp,RESP_MSG_SIZE,0,0);
        if(msgLen==-1){
            perror("msgrcv");
            exit(1);
        }
        totBytes+=msgLen;
    }

    printf("Received %d bytes (%d messages)\n",totBytes,numMsgs);
}