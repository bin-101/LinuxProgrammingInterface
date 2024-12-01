#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "socket_library.h"


#define PORT_NUM "50000"
#define INT_LEN 30
#define BACKLOG 50

/*
例59-6を参考にする
*/

int main() {

    if(signal(SIGPIPE,SIG_IGN)==SIG_ERR){
        perror("signal");
    }

    socklen_t socklen;
    int fd=inetListen(PORT_NUM,BACKLOG,&socklen);
    if(fd==-1){
        return 1;
    }
    int seqNum=0;
    while(TRUE){
        socklen_t addrlen=sizeof(struct sockaddr_storage);
        struct sockaddr_storage perr_addr;
        int cfd=accept(fd, (struct sockaddr *)&perr_addr, &addrlen);
        if(cfd==-1){
            perror("accept");
            return 1;
        }
        #define ADDRSTRLEN (NI_MAXHOST+NI_MAXSERV+10)
        char addrStr[ADDRSTRLEN];
        inetAddressStr((struct sockaddr *)&perr_addr,addrlen,addrStr,ADDRSTRLEN);
        printf("Connection from %s\n",addrStr);

        char reqLenStr[INT_LEN];
        if(readLine(cfd,reqLenStr,INT_LEN)<=0){
            close(cfd);
            continue;
        }
        int reqLen=atoi(reqLenStr);
        if(reqLen<=0){
            close(cfd);
            continue;
        }

        char seqNumStr[INT_LEN];
        snprintf(seqNumStr,INT_LEN,"%d\n",seqNum);
        if(write(cfd,seqNumStr,strlen(seqNumStr))!=strlen(seqNumStr)){
            fprintf(stderr,"Error on write");
        }
        seqNum+=reqLen;
        if(close(cfd)==-1){
            perror("close");
        }
    }
}