#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*
理由
O_APPENDを指定すると、ファイルオフセットの末尾への移動と書き込みをアトミックに行うため。
*/

int main(int argc, char *argv[])
{
    //a-zまでの文字が書き込まれたファイルを作成
    const int SIZE=26;
    char buffer[SIZE];
    for (int i=0; i<SIZE; i++){
        buffer[i]='a'+i;
    }
    int fd1=open("test1.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
    int fd2=open("test2.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
    write(fd1, buffer, SIZE);
    write(fd2, buffer, SIZE);
    close(fd1);
    close(fd2);

    int fd_normal=open("test1.txt", O_WRONLY);
    int fd_append=open("test2.txt", O_WRONLY | O_APPEND);
    char buffer2[]={'x','y'};
    write(fd_normal, buffer2, 2);
    write(fd_append, buffer2, 2);
    close(fd_normal);
    close(fd_append);

    return 0;
}