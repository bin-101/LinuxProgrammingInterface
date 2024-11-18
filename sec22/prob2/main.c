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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec22/prob2$ ./a.out
NSIG: 65
SIGRTMIN: 34
SIGRTMAX: 64
Signal 4 received
Signal 5 received
Signal 7 received
Signal 8 received
Signal 11 received
Signal 31 received
Signal 1 received
Signal 2 received
Signal 3 received
Signal 6 received
Signal 10 received
Signal 12 received
Signal 13 received
Signal 14 received
Signal 15 received
Signal 16 received
Signal 17 received
Signal 20 received
Signal 21 received
Signal 22 received
Signal 23 received
Signal 24 received
Signal 25 received
Signal 26 received
Signal 27 received
Signal 28 received
Signal 29 received
Signal 30 received
Signal 34 received
Signal 35 received
Signal 36 received
Signal 37 received
Signal 38 received
Signal 39 received
Signal 40 received
Signal 41 received
Signal 42 received
Signal 43 received
Signal 44 received
Signal 45 received
Signal 46 received
Signal 47 received
Signal 48 received
Signal 49 received
Signal 50 received
Signal 51 received
Signal 52 received
Signal 53 received
Signal 54 received
Signal 55 received
Signal 56 received
Signal 57 received
Signal 58 received
Signal 59 received
Signal 60 received
Signal 61 received
Signal 62 received
Signal 63 received
Signal 64 received

- リアルタイムシグナルをシグナル番号の昇順で受信していることを確かめた
- リアルタイムシグナルより標準シグナルの方を先に受信することが分かった
*/

void handler(int sig){
    printf("Signal %d received\n", sig);
}

int main(){
    printf("NSIG: %d\n", NSIG);
    printf("SIGRTMIN: %d\n", SIGRTMIN);
    printf("SIGRTMAX: %d\n", SIGRTMAX);

    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask); // すべてのシグナルをブロックする必要がある。そうしないと、シグナルハンドラ実行中に他のシグナルが割り込んでしまう
    for(int i=1;i<=NSIG;i++){
        sigaction(i, &act, NULL);
    }

    // すべてをブロック
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_SETMASK, &set, NULL);

    // シグナルを送信
    for(int i=1;i<=NSIG;i++){
        if(i==SIGKILL || i==SIGSTOP) continue;
        raise(i);
    }

    sigprocmask(SIG_UNBLOCK, &set, NULL);
}