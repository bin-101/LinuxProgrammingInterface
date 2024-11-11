#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec20/prob2$ ./a.out 
2 is not pending
2 is not pending
^C2 is pending
2 is pending
2 is pending
2 is pending
2 is pending
2 is pending
2 is pending
2 is pending
2 is not pending
*/

void check_pending(int sig)
{
    sigset_t pending;
    sigpending(&pending);
    if(sigismember(&pending, sig)){
        printf("%d is pending\n", sig);
    }else{
        printf("%d is not pending\n", sig);
    }
}

int main(int argc, char *argv[])
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT); //SIGINT: 割り込み処理(ctrl+c)
    int x=sigprocmask(SIG_BLOCK, &sigset, NULL);
    if(x==-1){
        printf("sigprocmask: %s\n", strerror(errno));
    }
    for(int i=0;i<10;i++){
        check_pending(SIGINT);
        sleep(1);
    }

    struct sigaction act;
    act.sa_handler=SIG_IGN;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    check_pending(SIGINT);
}