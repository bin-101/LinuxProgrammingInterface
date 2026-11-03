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
#include "../../sec59/prob2/socket_library.h"

/*
出力結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob4$ ./a.out
(::, 48283)
(::, 40915)
(::, 51029)
(::, 55699)
(::, 53977)
(::, 36207)
(::, 54383)
(::, 48215)
(::, 51401)
(::, 35637)
(0.0.0.0, 54053)
(0.0.0.0, 34133)
(0.0.0.0, 35837)
(0.0.0.0, 36113)
(0.0.0.0, 47073)
(0.0.0.0, 39073)
(0.0.0.0, 52813)
(0.0.0.0, 56587)
(0.0.0.0, 34427)
(0.0.0.0, 57411)
*/

void print(int sfd){
    if(listen(sfd,10)==-1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_storage addr;
    socklen_t addr_len=sizeof(addr);
    if(getsockname(sfd,(struct sockaddr *)&addr,&addr_len)==-1){
        printf("getsockname: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    char str[100];
    printf("%s\n",inetAddressStr((struct sockaddr *)&addr,addr_len,str,100));
}

int main() {
    for(int i=0;i<10;i++){
        int sfd=socket(AF_INET6, SOCK_STREAM, 0);
        print(sfd);
    }
    for(int i=0;i<10;i++){
        int sfd=socket(AF_INET, SOCK_STREAM, 0);
        print(sfd);
    }
}