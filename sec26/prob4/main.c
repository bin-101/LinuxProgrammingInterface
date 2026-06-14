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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob4$ ./a.out
Parent PID=70479
Child (PID=70480) exiting
Signal 17 (Child exited) received
  70479 pts/2    00:00:00 a.out
  70480 pts/2    00:00:00 a.out
After sending SIGKILL to zombie (PID=70480):
  70479 pts/2    00:00:00 a.out
  70480 pts/2    00:00:00 a.out
*/

#define CMD_SIZE 200

void handler(int sig){
    printf("Signal %d (%s) received\n",sig,strsignal(sig));
}


void wait_signal(int signal_id, void (*handler)(int)){
    struct sigaction sa;
    sa.sa_handler=handler;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    if(sigaction(signal_id, &sa, NULL)==-1){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    // signal_idのシグナルが来るまで待つ sigsuspend
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, signal_id);
    if(sigsuspend(&mask)==-1 && errno!=EINTR){
        perror("sigsuspend");
        exit(EXIT_FAILURE);
    }
}

void block_all_signals(){
    sigset_t mask;
    sigfillset(&mask);
    if(sigprocmask(SIG_SETMASK, &mask, NULL)==-1){
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }   
}


int main(int argc,char *argv[]){


    setbuf(stdout,NULL);

    printf("Parent PID=%ld\n",(long)getpid());

    pid_t childPid;

    block_all_signals();

    switch(childPid=fork()){
        case -1:
            printf("fork: %s\n",strerror(errno));
        case 0: // 子プロセス
            printf("Child (PID=%ld) exiting\n",(long)getpid());
            _exit(EXIT_SUCCESS);
        default: // 親プロセス
            wait_signal(SIGCHLD, handler); // SIGCHLDを受信するまで待つ
            char cmd[CMD_SIZE];
            snprintf(cmd,CMD_SIZE,"ps | grep %s",basename(argv[0]));
            system(cmd);

            if(kill(childPid,SIGKILL)==-1){
                printf("kill: %s\n",strerror(errno));
            }
            sleep(3);
            printf("After sending SIGKILL to zombie (PID=%ld):\n",(long)childPid);
            system(cmd);
    }
}