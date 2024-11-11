#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>

/*
SA_NODEFERの効果を確かめる
- SA_NODEFER: シグナルハンドラが実行される間に、同じシグナルがブロックされない

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec20/prob3$ ./a.out
^Ccnt: 1
^C^Ccnt: 2 //2回SIGINTを送っているが、追加で1回しか実行されない
end
^Ccnt: 3
^Ccnt: 4
^Ccnt: 5
^Ccnt: 6
^Ccnt: 7
*/

int cnt=0;

//1回目のpause(): ctrl+cを押すと、handlerが実行された後、次のhandlerが実行される
//2回目のpause(): ctrl+cを押すと、すぐに次のhandlerが実行される
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