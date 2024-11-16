#define _GNU_SOURCE
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

/*
実行結果: 何も表示されない

実行結果(\nを追加)
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec27/prob5$ ./a.out
Hello World

- 2つの原因が考えられる
    1. stdioバッファをflushしていない
    2. execlp()でstdioバッファの情報を削除している
- exit(0)を実行するプログラム("exit.c")をexitlpで実行すると、何も表示されなかった。よって、原因は2である
*/

int main(int argc, char *argv[]){
    printf("Hello World");
    execlp("exit0","exit0",(char *) NULL);
    execlp("sleep","sleep","0",(char *) NULL);
}