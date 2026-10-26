#include "../../sec59/prob2/socket_library.h"

#define BUF_SIZE 100

// 61-2を写経した

int main(int argc,char *argv[]){
    if(argc!=2){
        printf("Usage: %s <hostname>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int sfd=inetConnect(argv[1],"echo",SOCK_STREAM);
    if(sfd==-1){
        printf("inetConnect: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    switch(fork()){
        case -1:
            printf("fork: %s\n",strerror(errno));
            exit(EXIT_FAILURE);
        case 0: // 子プロセス
            while(1){
                char buf[BUF_SIZE];
                ssize_t numRead;
                numRead=read(STDIN_FILENO,buf,BUF_SIZE);
                if(numRead<=0){
                    break;
                }
                printf("numRead: %.*s\n", (int)numRead, buf);
            }
            exit(EXIT_SUCCESS);
        default: // 親プロセス
            while(1){
                char buf[BUF_SIZE];
                ssize_t numRead;
                numRead=read(sfd,buf,BUF_SIZE);
                if(numRead<=0){
                    break;
                }
                if(write(sfd,buf,numRead)!=numRead){
                    printf("write: %s\n",strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }
            if(shutdown(sfd,SHUT_WR)==-1){
                printf("shutdown: %s\n",strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
    }
}