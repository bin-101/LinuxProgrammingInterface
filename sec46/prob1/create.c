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

/*
実際に動かしてみる (MAX_MTEXTは1024)
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ ./create -c -f ./test.txt
perms: 384
21
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ ./send 21 20 aiueo
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ ./receive -t -19 21
^C
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ ./receive -t -20 21
Received: type=20; length=6; body=aiueo

receive.cのMAX_MTEXTを2に変更
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ gcc receive.c -oreceive
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ ./send 21 20 aiueo
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ ./receive -t -20 21
Received: type=140729148212185; length=-1 # おそらくE2BIGのエラーを返している
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ ./receive -e -t -20 21 # -e (MSG_NOERROR)を追加
Received: type=20; length=2; body=ai

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ ./send 21 20 aiueo
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob1$ ipcs

------ メッセージキュー --------
キー     msqid      所有者  権限     使用済みバイト数 メッセージ
0x00000000 0          bin101     0          0            0
(省略)
0x0107530a 21         bin101     600        6            1　# 使用済みバイト数は6らしい
*/

// 例46-1 svmsg_create.c の写経

int main(int argc, char *argv[]){
    int numKeyFlags=0;
    int flags=0;
    key_t key;
    long lkey;
    unsigned int perms=(1<<30);

    int opt;
    while((opt=getopt(argc,argv,"cf:k:px"))!=-1){
        switch(opt){
            case 'c':
                flags|=IPC_CREAT;
                break;
            case 'f':
                key=ftok(optarg,1);
                if(key==-1){
                    printf("ftok: %s\n",strerror(errno));
                    exit(1);
                }
                numKeyFlags++;
                break;
            case 'k':
                if(sscanf(optarg,"%li",&lkey)!=-1){
                    printf("sscanf: %s\n",strerror(errno));
                }
                key=lkey;
                numKeyFlags++;
                break;
            case 'p':
                key=IPC_PRIVATE;
                numKeyFlags++;
                break;
            case 'x':
                flags|=IPC_EXCL;
                break;
            default:
                printf("default\n");
                break;
        }
    }

    if(numKeyFlags!=1){
        printf("numkeyFlags!=1\n");
    }
    perms=(optind==argc)?(S_IRUSR|S_IWUSR):atoi(argv[optind]);
    printf("perms: %d\n", perms);

    int msqid=msgget(key,flags|perms);
    if(msqid==-1){
        printf("msgget: %s\n",strerror(errno));
        exit(1);
    }

    printf("%d\n",msqid);
}
