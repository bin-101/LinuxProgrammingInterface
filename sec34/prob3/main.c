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
#include <sys/time.h>
#include <pthread.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec34/prob3$ ./a.out
parent process pid: 11907 pgid: 11907
sid: -1　# グループリーダのsetsid()はエラーとなる
child process pid: 11908 pgid: 11907
sid: 11908
*/

int main(){
    printf("parent process pid: %d pgid: %d\n", getpid(),getpgrp());
    pid_t sid=setsid();
    printf("sid: %d\n", sid);

    switch(fork()){
        case 0:
            printf("child process pid: %d pgid: %d\n", getpid(),getpgrp());
            pid_t sid = setsid();
            printf("sid: %d\n", sid);
            break;
        default:
            break;
    }
}