#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>

/*
SA_RESETHAND: シグナルハンドラが実行されると、そのシグナルのハンドラの動作をデフォルトに戻す

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec20/prob3$ ./a.out
^Ccnt: 1
^C^Ccnt: 2
end
^Ccnt: 3
^C // デフォルトの動作で終了するため、endが出力されない
*/

int cnt=0;

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
    pause();
    printf("end\n");

    act.sa_handler=handler;
    act.sa_flags=SA_RESETHAND;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    pause();
    printf("end\n"); //2回目のhandler実行中にctrl+cを押すと、この行は実行されない
}