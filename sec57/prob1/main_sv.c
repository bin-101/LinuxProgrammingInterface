#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>

#define SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100
#define BACKLOG 5

/*
例57-3を参考にする

信頼性: 内容が変更されない。失敗したらその旨が通知される
*/

int main() {
    int sfd=socket(AF_UNIX, SOCK_STREAM, 0);

    if(remove(SOCK_PATH)==-1 && errno!=ENOENT){ // ENOENT: No such file or directory
        perror("remove");
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family=AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);

    if(bind(sfd,(struct sockaddr *)&addr,sizeof(struct sockaddr_un))==-1){
        perror("bind");
    }

    if(listen(sfd,BACKLOG)==-1){
        perror("listen");
    }

    while(1){
        int cfd=accept(sfd,NULL,NULL);

        ssize_t numRead;
        char buf[BUF_SIZE];
        while((numRead=read(cfd, buf, BUF_SIZE))>0){
            if(write(STDOUT_FILENO, buf, numRead)!=numRead){
                perror("partial/failed write");
                return 1;
            }
            sleep(1); // 1秒待つ
        }

        if(numRead==-1){
            perror("read");
            return 1;
        }

        if(close(cfd)==-1){
            perror("close");
            return 1;
        }
    }
}