#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "../prob2/socket_library.h"

#define PORT_NUM "50000"
#define BACKLOG 50
#define MAXDATA 1000
#define ADDRSTRLEN (NI_MAXHOST+NI_MAXSERV+10)
#define REQUEST_LEN (5+30+10)
#define BUF_SIZE 100

int main(int argc, char *argv[]){
    int sfd=inetConnect(argv[1],PORT_NUM,SOCK_STREAM);
    if(sfd==-1){
        return 1;
    }

    ssize_t numRead;
    char buf[BUF_SIZE];
    while((numRead=read(STDIN_FILENO, buf, BUF_SIZE))>0){
        if(write(sfd, buf, numRead)!=numRead){
            perror("partial/failed write");
            return 1;
        }
        if(readLine(sfd,buf,BUF_SIZE)<=0){
            perror("readLine");
            return 1;
        }
        if(write(STDOUT_FILENO,buf,strlen(buf))!=strlen(buf)){
            perror("partial/failed write");
            return 1;
        }
    }
    
    return 0;
}