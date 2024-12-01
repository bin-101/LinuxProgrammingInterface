#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#define SOCK_PATH "\0/tmp/us_xfr"
#define BUF_SIZE 100

int main() {
    int sfd=socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family=AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);

    if(connect(sfd,(struct sockaddr *)&addr, sizeof(struct sockaddr_un))==-1){
        perror("connect");
        return 1;
    }

    ssize_t numRead;
    char buf[BUF_SIZE];
    while((numRead=read(STDIN_FILENO, buf, BUF_SIZE))>0){
        if(write(sfd, buf, numRead)!=numRead){
            perror("partial/failed write");
            return 1;
        }
    }

    if(numRead==-1){
        perror("read");
        return 1;
    }

    return 0;
}