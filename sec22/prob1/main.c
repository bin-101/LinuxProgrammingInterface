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

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec22/prob1$ ./a.out
PID: 35310
cnt: 0
cnt: 0
cnt: 0
cnt: 0

[1]+  停止                  ./a.out # kill -SIGSTOP 35310を実行
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec22/prob1$ fg
./a.out
cnt: 0
cnt: 0
cnt: 0
Signal 15 received　# kill -SIGTERM 35310を実行。
Signal 18 received
cnt: 1
cnt: 1
^C
*/

volatile sig_atomic_t cnt_cont=0;
volatile sig_atomic_t cnt_term=0;


void handler(int sig){
    cnt_cont++;
    printf("Signal %d received\n", sig);
}

void handler2(int sig){
    cnt_term++;
    printf("Signal %d received\n", sig);
}

int main(){
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCONT, &sa, NULL);

    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGCONT);
    sigprocmask(SIG_BLOCK, &sigmask, NULL);

    struct sigaction sa_term;
    sa_term.sa_handler = handler2;
    sa_term.sa_flags = 0;
    sigemptyset(&sa_term.sa_mask);
    sigaction(SIGTERM, &sa_term, NULL);

    printf("PID: %d\n", getpid());

    while(1){
        printf("cnt: %d\n", cnt_cont);
        sleep(5);
        if(cnt_term){
            sigset_t sigmask;
            sigemptyset(&sigmask);
            sigprocmask(SIG_SETMASK, &sigmask, NULL);
        }
    }
}