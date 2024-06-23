#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>

int cnt=0;

//1回目: ctrl+cを押すと、handlerが実行された後、次のhandlerが実行される
//2回目: ctrl+cを押すと、すぐに次のhandlerが実行される
void handler(int sig)
{
    cnt++;
    printf("cnt: %d\n", cnt);
    sleep(3);
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler=handler;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    pause(); //プロセスがシグナルを受け取るまで停止
    printf("end\n");

    act.sa_handler=handler;
    act.sa_flags=SA_NODEFER;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    pause();
}