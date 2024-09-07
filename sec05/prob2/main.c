#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*
fd=open("test.txt", O_WRONLY | O_APPEND);
lseek(fd, 0, SEEK_SET);
- 問題文の通り
- abcdefghijklmnopqrstuvwxyzxy

fd=open("test.txt", O_WRONLY);
lseek(fd, 0, SEEK_SET);
- xycdefghijklmnopqrstuvwxyz

fd=open("test.txt", O_WRONLY | O_APPEND);
lseek(fd, x, SEEK_SET);
- x=10,100でも下の出力になった
- abcdefghijklmnopqrstuvwxyzxy

理由
O_APPENDを指定すると、末尾への移動と書き込みをアトミックに行うため。
*/

int main(int argc, char *argv[])
{
    //a-zまでの文字が書き込まれたファイルを作成
    const int SIZE=26;
    char buffer[SIZE+1];
    for (int i=0; i<SIZE; i++){
        buffer[i]='a'+i;
    }
    buffer[SIZE]='\0';
    int fd=open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    write(fd, buffer, SIZE);
    close(fd);

    fd=open("test.txt", O_WRONLY | O_APPEND);
    lseek(fd, 0, SEEK_SET);
    char buffer2[3]={'x','y','\0'};
    write(fd, buffer2, 2);
    close(fd);

    return 0;
}