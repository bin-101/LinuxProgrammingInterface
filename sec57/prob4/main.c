#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#define SOCK_PATH_A "\0tlpi_57_4_a"
#define SOCK_PATH_B "\0tlpi_57_4_b"

// bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec57/prob4$ ./a.out
// 3つ目のソケットからaへのsendto()は失敗: Operation not permitted


int main() {
    int a_sfd=socket(AF_UNIX, SOCK_DGRAM, 0);
    int b_sfd=socket(AF_UNIX, SOCK_DGRAM, 0);
    int third_sfd=socket(AF_UNIX, SOCK_DGRAM, 0);

    if(a_sfd==-1 || b_sfd==-1 || third_sfd==-1){
        perror("socket");
        return 1;
    }

    struct sockaddr_un addr_a;
    memset(&addr_a, 0, sizeof(struct sockaddr_un));
    addr_a.sun_family=AF_UNIX;
    memcpy(addr_a.sun_path, SOCK_PATH_A, sizeof(SOCK_PATH_A)-1);
    socklen_t addr_a_len=offsetof(struct sockaddr_un, sun_path)
                          +sizeof(SOCK_PATH_A)-1;

    struct sockaddr_un addr_b;
    memset(&addr_b, 0, sizeof(struct sockaddr_un));
    addr_b.sun_family=AF_UNIX;
    memcpy(addr_b.sun_path, SOCK_PATH_B, sizeof(SOCK_PATH_B)-1);
    socklen_t addr_b_len=offsetof(struct sockaddr_un, sun_path)
                          +sizeof(SOCK_PATH_B)-1;

    if(bind(a_sfd,(struct sockaddr *)&addr_a,addr_a_len)==-1){
        perror("bind a");
        return 1;
    }

    if(bind(b_sfd,(struct sockaddr *)&addr_b,addr_b_len)==-1){
        perror("bind b");
        return 1;
    }

    if(connect(a_sfd,(struct sockaddr *)&addr_b,addr_b_len)==-1){
        perror("connect a to b");
        return 1;
    }

    char buf[]="hello";
    ssize_t numSent=sendto(third_sfd,buf,strlen(buf),0,
                           (struct sockaddr *)&addr_a,
                           addr_a_len);

    if(numSent==-1){
        printf("3つ目のソケットからaへのsendto()は失敗: %s\n",
               strerror(errno));
    }else{
        printf("3つ目のソケットからaへのsendto()は成功: %ld bytes\n",
               (long)numSent);
    }

    if(close(a_sfd)==-1 || close(b_sfd)==-1 || close(third_sfd)==-1){
        perror("close");
        return 1;
    }

    return 0;
}
