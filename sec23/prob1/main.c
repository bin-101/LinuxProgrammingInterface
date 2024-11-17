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
- int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
    - 返り値: 成功時0, 失敗時-1
    - 時間が経ったらシグナルを送る
- unsigned int alarm(unsigned int seconds);
    - 返り値: 0または前回のalarm()の残り時間
    - 時間が経ったらシグナルを送る

考察
- whichはITIMER_REALでよさそう(送るシグナル的にもタイマー的にも)

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec23/prob1$ ./a.out
sec=4
Caught signal 14
Alarm clock
*/

unsigned int my_alarm(unsigned int seconds){
    struct itimerval new_value, old_value;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_usec = 0;
    new_value.it_value.tv_sec = seconds;
    new_value.it_value.tv_usec = 0;
    if(setitimer(ITIMER_REAL, &new_value, &old_value)==-1){
        printf("setitimer: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return old_value.it_value.tv_sec;
}

void handler(int sig){
    printf("Caught signal %d\n", sig);
    printf("%s\n", strsignal(sig));
}

int main(int argc, char *argv[]){
    int sec=my_alarm(5);
    sec=my_alarm(3);
    printf("sec=%d\n", sec);
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGALRM, &sa, NULL)==-1){
        printf("sigaction: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    pause();
}