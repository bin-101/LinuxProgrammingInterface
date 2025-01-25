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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec22/prob4$ ./a.out
^Csig: 2
sleep(3) end
^C^C^Csig: -1

- sighandler_t f=_sigset(SIGINT,SIG_HOLD); でsigintがブロックされたことを確認できた
- f でハンドラを受け取っていることを確認できた
*/

// sigをプロセスのシグナルマスクへ追加
int _sighold(int sig){
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset,sig);
    return sigprocmask(SIG_BLOCK,&sigset,NULL);
}

// sigをプロセスのシグナルマスクから削除
int _sigrelse(int sig){
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset,sig);
    return sigprocmask(SIG_UNBLOCK,&sigset,NULL);
}

// シグナル動作を無視へ変更
int _sigignore(int sig){
    struct sigaction sa;
    sa.sa_handler=SIG_IGN;
    return sigaction(sig,&sa,NULL);
}


int _sigpause(int sig){
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset,sig);
    return sigsuspend(&sigset);
}

typedef void (*sighandler_t)(int); // p419

sighandler_t _sigset(int sig, sighandler_t handler){
    if(handler==SIG_HOLD){
        if(_sighold(sig)==-1) return (sighandler_t)-1;
        // man page: sigaction()  can be called with a NULL second argument to query the current signal handler.
        struct sigaction sa;
        if(sigaction(sig,NULL,&sa)==-1) return (sighandler_t)-1;
        return sa.sa_handler;
    }

    sigset_t now_sigmask;
    if(sigprocmask(SIG_BLOCK,NULL,&now_sigmask)==-1){
        return (sighandler_t)-1;
    }
    int is_blocked=sigismember(&now_sigmask,sig);
    if(is_blocked==-1) return (sighandler_t)-1;
    if(is_blocked){
        if(_sigrelse(sig)==-1) return (sighandler_t)-1;
    }

    struct sigaction sa;
    sa.sa_handler=handler;
    struct sigaction old_sa;
    if(sigaction(sig,&sa,&old_sa)==-1){
        if(is_blocked) _sighold(sig);
        return (sighandler_t)-1;
    }
    if(is_blocked) return SIG_HOLD;
    return old_sa.sa_handler;
}

void handler(int sig){
    printf("sig: %d\n",sig);
}

int main(){
    _sigset(SIGINT,handler);
    sleep(3);
    printf("sleep(3) end\n");
    sighandler_t f=_sigset(SIGINT,SIG_HOLD);
    sleep(3);
    f(-1);
}