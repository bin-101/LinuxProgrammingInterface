#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
実行結果
test.txt
fd2
fd1

標準出力
fd1=3
fd2=4
flag1=32770
flag2=32770

以下のことを確認できた。
- ファイルオフセットを共有している
    - fd2の次にfd1が出力されているため
- オープンファイルステータスフラグが共有されている
    - fd1とfd2のフラグが同じであるため
*/

int main(int argc, char *argv[])
{
    int fd1=open("test.txt",O_RDWR|O_CREAT|O_TRUNC,0777);
    printf("fd1=%d\n",fd1);
    int fd2=dup(fd1);
    printf("fd2=%d\n",fd2);

    int flag1=fcntl(fd1,F_GETFL);
    int flag2=fcntl(fd2,F_GETFL);

    printf("flag1=%d\n",flag1);
    printf("flag2=%d\n",flag1);

    write(fd2,"fd2\n",4);
    write(fd1,"fd1\n",4);

    close(fd1);
    close(fd2);
}
