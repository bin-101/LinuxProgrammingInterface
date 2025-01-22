#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *file="test.txt";
    int fd1=open(file,O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
    int fd2=dup(fd1);
    int fd3=open(file,O_RDWR);

    write(fd1,"Hello,",6);
    write(fd2," world",6); // fd1の続きに書き込む
    lseek(fd2,0,SEEK_SET); // fd1とfd2のファイルオフセットを0にする
    write(fd1,"HELLO,",6);
    write(fd3,"Gidday",6); // fd3は何も操作していないので、ファイルオフセットが0のところに書き込む

    close(fd1);
    close(fd2);
    close(fd3);
}
