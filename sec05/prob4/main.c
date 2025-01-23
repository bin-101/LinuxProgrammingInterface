#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
以下のように出力される。
fd
fd2
fd3

以下のことを確認できた。
- ファイルディスクリプタが同じファイルを指している
- ファイルオフセットが共有されている
*/

int mydup(int oldfd){
    return fcntl(oldfd, F_DUPFD, 0);
}

int mydup2(int oldfd,int newfd){
    if(fcntl(oldfd, F_GETFL) == -1){ //flagを返す。oldfdが有効ではない場合、エラーになり-1を返す
        errno=EBADF;
        return -1;
    }
    if(oldfd == newfd){
        return oldfd;
    }
    if(fcntl(newfd, F_GETFL) != -1){ //newfdが有効な場合、newfdを閉じる
        close(newfd);
    }
    return fcntl(oldfd, F_DUPFD, newfd);
}

int main(int argc, char *argv[])
{
    int fd=open("test.txt",O_RDWR|O_CREAT|O_TRUNC,0777);
    printf("fd=%d\n",fd);
    int fd2=mydup(fd);
    printf("fd2=%d\n",fd2);
    int fd3=mydup2(fd,0);
    printf("fd3=%d\n",fd3);
    write(fd,"fd\n",3);
    write(fd2,"fd2\n",4);
    write(fd3,"fd3\n",4);
    close(fd);
    close(fd2);
    close(fd3);
}
