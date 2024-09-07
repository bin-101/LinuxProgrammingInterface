#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/*
実行結果
```shell
f
f
Segmentation fault
```
*/
static jmp_buf env;

int f(){
    setjmp(env);
    printf("f\n");
}

int main(int argc, char *argv[])
{
    f();
    longjmp(env, 1);
}