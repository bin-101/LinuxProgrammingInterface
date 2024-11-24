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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec34/prob1$ ./a.out | cat
Parent 8073
Child 8075 (getpgrep: 8073)
Child 8075 loop: 0
Child 8076 (getpgrep: 8073)
Child 8076 loop: 0
Child 8075 loop: 1
Child 8076 loop: 1
Parent end
ユーザー定義シグナル1

問題点
catを実行するプロセスと./a.outを実行するプロセスは同じプロセスグループに属しているため、
catにもSIGUSR1が送られて終了する。

回避策
setpgid()を使って子プロセスのプロセスグループを変更する。
*/

int main(){
    setvbuf(stdout,NULL,_IONBF,0);
    printf("Parent %d\n",getpid());

    for(int i=0;i<2;i++){
        switch(fork()){
            case -1:
                printf("fork: %s",strerror(errno));
                exit(1);
            case 0:
                printf("Child %d (getpgrep: %d)\n",getpid(),getpgrp());
                for(int j=0;j<1000;j++){
                    printf("Child %d loop: %d\n",getpid(),j);
                    sleep(1);
                }
                exit(1);
            default:
                break;
        }
    }

    sleep(2);

    signal(SIGUSR1,SIG_IGN); // SIGUR1を無視する

    killpg(getpid(),SIGUSR1); // 自分のプロセスグループにSIGUSR1を送る。SIGUSR1のディフォルト動作は終了

    printf("Parent end\n"); // これは出力されない
    fprintf(stderr,"Parent end\n"); // これは出力される
}