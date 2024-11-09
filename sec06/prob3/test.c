#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/*
調べたいこと
- 存在しない環境変数をunsetenvした場合、返される値は何か
- overwrite=0の際、setenvで既に存在する環境変数を指定したら返される値は何か
- 上書きすると、environの中身はどうなっているのか

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec06/prob3$ ./a.out
0
0
0
0
~略~
HOGE=hoge2
*/

extern char **environ;

void print_env()
{
    char **ep=environ;
    while(*ep!=NULL){
        printf("%s\n",*ep);
        ep++;
    }
}


int main(int argc, char *argv[])
{
    printf("%d\n",unsetenv("HOGE"));
    printf("%d\n",setenv("HOGE", "hoge0", 0));
    printf("%d\n",setenv("HOGE", "hoge1", 0));
    printf("%d\n",setenv("HOGE", "hoge2", 1));
    print_env();

}
