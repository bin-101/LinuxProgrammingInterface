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

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob2$ ./sv
msqId: 27
clientPid: 54520
seqNum: 0
clientPid: 54549
seqNum: 7
clientPid: 54565
seqNum: 14
clientPid: 54573
seqNum: 20

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob2$ ./cl 7
msqId: 27
getpid(): 54520
seq_num: 0
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob2$ ./cl 7
msqId: 27
getpid(): 54549
seq_num: 7
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob2$ ./cl 6
msqId: 27
getpid(): 54565
seq_num: 14
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob2$ ./cl 5
msqId: 27
getpid(): 54573
seq_num: 20

ipcsでid27のメッセージキューが削除されていることを確認
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob2$ ipcs

------ メッセージキュー --------
キー     msqid      所有者  権限     使用済みバイト数 メッセージ
0x00000000 12         bin101     7          0            0           
0x00000000 13         bin101     7          0            0           
0x00000000 14         bin101     7          0            0           
0x00000000 15         bin101     7          0            0           
0x00000000 16         bin101     7          0            0           
0x00000000 17         bin101     0          0            0           
0x00000000 18         bin101     0          0            0           
0x00000000 19         bin101     0          0            0
*/

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

void removeMsq(int status,void *msqid){
    int msqid_int=(int)msqid;
    msgctl(msqid_int,IPC_RMID,NULL);
}

int main(int argc,char *argv[]){
    int seqNum=0;

    // メッセージキュー作成
    int msqId=msgget(ftok("./test.txt",1),IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
    printf("msqId: %d\n",msqId);
    if(msqId==-1){
        printf("msgget: %s\n",strerror(errno));
        exit(1);
    }
    if(on_exit(removeMsq,(void *)msqId)!=0){
        printf("atexit: %s\n",strerror(errno));
        exit(1);
    }

    int loop=5;
    while(1){
        loop--;
        if(loop==0) break;
        struct request req;
        int msgLen;
        if((msgLen=msgrcv(msqId,&req,8,1,0))==-1){
            perror("msgrcv");
            exit(1);
        }
        int clientPid=req.pid;
        printf("clientPid: %d\n",clientPid);

        struct response resp;
        resp.mtype=clientPid;
        resp.num=seqNum;
        printf("seqNum: %d\n",seqNum);
        if(msgsnd(msqId,&resp,4,0)==-1){
            perror("msgsend");
        }
        seqNum+=req.num;
    }
}
