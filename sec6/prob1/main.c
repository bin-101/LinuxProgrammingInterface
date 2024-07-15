#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*
gcc main.c
ls -l a.out
-rwxr-xr-x 1 hitoshi-murakami domain^users 16024 Jul  4 14:06 a.out

335Byte
globBufとmbufは未初期化データセグメント
未初期化変数にはディスク領域を与える必要がない
(プログラムロード時には領域を割り当てるらしい)
*/

char globBuf[65536];

int main(int argc, char *argv[])
{
    static char mbuf[1024000];

    for(int i=0;i<sizeof(mbuf);i++){
        printf("%d\n",mbuf[i]);
    }

    for(int i=0;i<sizeof(globBuf);i++){
        printf("%d\n",globBuf[i]);
    }
}