#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#define _GNU_SOURCE

// 例11-2 丸写し
/*
実行結果
_PC_NAME_MAX:  255
_PC_PATH_MAX:  4096
_PC_PIPE_BUF:  4096
*/
static void fpathconfPrint(const char *msg, int fd, int name){
    long lim;

    errno = 0;
    lim = fpathconf(fd, name);
    if (lim != -1){
        printf("%s %ld\n", msg, lim);
    } else{
        if (errno == 0){
            printf("%s (indeterminate)\n",msg);
        }else{
            assert(0);
        }
    }
}

void main(){
    fpathconfPrint("_PC_NAME_MAX: ", STDIN_FILENO, _PC_NAME_MAX);
    fpathconfPrint("_PC_PATH_MAX: ", STDIN_FILENO, _PC_PATH_MAX);
    fpathconfPrint("_PC_PIPE_BUF: ", STDIN_FILENO, _PC_PIPE_BUF);
}