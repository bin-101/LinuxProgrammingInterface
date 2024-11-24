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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec34/prob2$ ./a.out
parent process: 11174
child process: 11175
0 getpgrp: 11174 # 最初は親プロセスのプロセスID
Signal: 10 process: 11174
0 res: 0
Signal: 10 process: 11175
1 getpgrp: 11175
Signal: 10 process: 11174
1 res: 0
parent process: 11174
Signal: 10 process: 11175
2 getpgrp: 3925 # 同一セッションのプロセスグループならば移動可能
2 res: -1 # exec()実行後は失敗する 
*/

void handler(int sig){
    printf("Signal: %d process: %d\n", sig, getpid());
}

int main(){
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);

    sigset_t empty_mask;
    sigemptyset(&empty_mask);

    int child = fork();
    if(child==0){
        printf("0 getpgrp: %d\n", getpgrp());
        kill(getppid(), SIGUSR1);
        sigsuspend(&empty_mask);
        printf("1 getpgrp: %d\n", getpgrp());
        kill(getppid(), SIGUSR1);
        sigsuspend(&empty_mask);
        printf("2 getpgrp: %d\n", getpgrp());
        kill(getppid(), SIGUSR1);

        // sleepを実行
        execlp("sleep", "sleep", "10", NULL);
        exit(0);
    }
    printf("parent process: %d\n", getpid());
    printf("child process: %d\n", child);
    
    sigsuspend(&empty_mask);
    int res=setpgid(child, child);
    printf("0 res: %d\n", res);
    kill(child, SIGUSR1);
    sigsuspend(&empty_mask);
    res=setpgid(child, getppid());
    printf("1 res: %d\n", res);
    printf("parent process: %d\n", getpid());
    kill(child, SIGUSR1);
    sleep(3);
    res=setpgid(child, 0);
    printf("2 res: %d\n", res);
}