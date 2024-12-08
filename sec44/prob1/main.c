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

#define BUF_SIZE 1024

// 例44-1を参考にする

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec44/prob1$ ./a.out
fafaf
FAFAF
yofiyafoaiofafu
YOFIYAFOAIOFAFU
^C
*/


int main(){
    int to_child_filedes[2];
    int to_parent_filedes[2];
    if(pipe(to_child_filedes)==-1 || pipe(to_parent_filedes)==-1){
        printf("pipe: %s\n",strerror(errno));
        exit(1);
    }

    switch(fork()){
        case -1:
            printf("fork: %s\n",strerror(errno));
            exit(1);
        case 0: // 子プロセス
            if(close(to_child_filedes[1])==-1 || close(to_parent_filedes[0])==-1){
                printf("close: %s\n",strerror(errno));
                exit(1);
            }
            while(1){
                char buf[BUF_SIZE];
                ssize_t numRead;
                numRead=read(to_child_filedes[0],buf,BUF_SIZE);
                if(numRead==-1){
                    printf("read: %s\n",strerror(errno));
                    exit(1);
                }
                if(numRead==0){
                    break;
                }
                for(int i=0;i<numRead;i++){
                    buf[i]=toupper(buf[i]);
                }
                if(write(to_parent_filedes[1],buf,numRead)!=numRead){
                    printf("write: %s\n",strerror(errno));
                    exit(1);
                }
            }
            _exit(0);
        default: // 親プロセス
            if(close(to_child_filedes[0])==-1 || close(to_parent_filedes[1])==-1){
                printf("close: %s\n",strerror(errno));
                exit(1);
            }
            while(1){
                char buf[BUF_SIZE];
                ssize_t numRead;
                numRead=read(STDIN_FILENO,buf,BUF_SIZE);
                if(numRead==-1){
                    printf("read: %s\n",strerror(errno));
                    exit(1);
                }
                if(numRead==0){
                    break;
                }
                if(write(to_child_filedes[1],buf,numRead)!=numRead){
                    printf("write: %s\n",strerror(errno));
                    exit(1);
                }
                if(read(to_parent_filedes[0],buf,numRead)!=numRead){
                    printf("read: %s\n",strerror(errno));
                    exit(1);
                }
                if(write(STDOUT_FILENO,buf,numRead)!=numRead){
                    printf("write: %s\n",strerror(errno));
                    exit(1);
                }
            }
            exit(0);
    }
}