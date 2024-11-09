#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec06/prob2$ ./a.out
f
f
Segmentation fault (コアダンプ)
*/
jmp_buf env;

int f(){
    setjmp(env);
    printf("f\n");
}

int main(int argc, char *argv[])
{
    f();
    longjmp(env, 1);
}