#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <limits.h>
#include <stddef.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec24/prob5$ ./a.out
0 [Mon Dec 11 17:11:44 115206
 77444] Parent about to wait for signal
0 [Tue Dec 17 22:01:45 2024
 77445] Child started
1 [Tue Dec 17 22:01:45 2024
 77445] Child about to signal parent
2 [Mon Dec 11 17:11:44 115206
 77444] Parent got signal
3 [Mon Dec 11 17:11:44 115206
 77444] Parent about to signal child
4 [Tue Dec 17 22:01:45 2024
 77445] Child got signal
*/

// 最初に47-2の解答をコピペした

#define SYNC_SIG SIGUSR1

static void
handler(int sig){

}

int
main(int argc,char *argv){
    setbuf(stdout,NULL);

    sigset_t blockMask,origMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask,SYNC_SIG);
    if(sigprocmask(SIG_BLOCK,&blockMask,&origMask)==-1){
        perror("sigprocmask");
        exit(1);
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    sa.sa_handler=handler;
    if(sigaction(SYNC_SIG,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }

    pid_t childPid;
    sigset_t emptyMask;
    switch(childPid=fork()){
        case -1:
            perror("fork");
            exit(1);
        case 0:
            time_t t=time(NULL);
            printf("0 [%s %d] Child started\n", ctime(&t), getpid());
            sleep(2);
            printf("1 [%s %d] Child about to signal parent\n", ctime(&t), getpid());
            if(kill(getppid(),SYNC_SIG)==-1){
                perror("kill");
                _exit(1);
            }
            sigemptyset(&emptyMask);            
            if(sigsuspend(&emptyMask)==-1 && errno!=EINTR){
                perror("sigprocmask");
                _exit(1);
            }
            printf("4 [%s %d] Child got signal\n", ctime(&t), getpid());
            _exit(0);
        default:
            printf("0 [%s %d] Parent about to wait for signal\n", ctime(&t), getpid());
            sigemptyset(&emptyMask);
            if(sigsuspend(&emptyMask)==-1 && errno!=EINTR){
                perror("sigsuspend");
                exit(1);
            }
            printf("2 [%s %d] Parent got signal\n", ctime(&t), getpid());

            if(sigprocmask(SIG_SETMASK,&origMask,NULL)==-1){
                perror("sigprocmask");
                exit(1);
            }
            sleep(2);
            printf("3 [%s %d] Parent about to signal child\n", ctime(&t), getpid());
            if(kill(childPid,SYNC_SIG)==-1){
                perror("kill");
                _exit(1);
            }

            exit(0);
    }
}
