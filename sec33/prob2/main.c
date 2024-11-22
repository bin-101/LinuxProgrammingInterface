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
数回実行した。子プロセスを作成していないスレッドも受け取るようだが、ほとんどのシグナルを子プロセスを作成しているスレッドが受け取った
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec33/prob2$ ./a.out
main thread: 130473312442176waitSignal: 130473307145792
makeChild: 130473296660032
signal: 17, thread: 130473296660032
signal: 17, thread: 130473296660032
signal: 17, thread: 130473312442176
signal: 17, thread: 130473296660032
signal: 17, thread: 130473296660032
signal: 17, thread: 130473296660032
signal: 17, thread: 130473296660032
signal: 17, thread: 130473296660032
signal: 17, thread: 130473296660032
sigmask: 130473296660032
signal: 17, thread: 130473312442176
signal: 17, thread: 130473312442176
signal: 17, thread: 130473312442176
signal: 17, thread: 130473307145792
signal: 17, thread: 130473312442176
signal: 17, thread: 130473312442176
signal: 17, thread: 130473307145792
signal: 17, thread: 130473307145792
signal: 17, thread: 130473307145792
signal: 17, thread: 130473307145792
signal: 17, thread: 130473312442176
^C
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec33/prob2$ gcc main.c
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec33/prob2$ ./a.out
main thread
: 140642591561536waitSignal: 140642585937472
makeChild: 140642575451712
signal: 17, thread: 140642575451712
signal: 17, thread: 140642575451712
signal: 17, thread: 140642575451712
signal: 17, thread: 140642575451712
signal: 17, thread: 140642575451712
signal: 17, thread: 140642575451712
signal: 17, thread: 140642575451712
signal: 17, thread: 140642575451712
sigmask: 140642575451712
signal: 17, thread: 140642591561536
signal: 17, thread: 140642591561536
signal: 17, thread: 140642585937472
signal: 17, thread: 140642585937472
signal: 17, thread: 140642585937472
signal: 17, thread: 140642585937472
signal: 17, thread: 140642591561536
signal: 17, thread: 140642591561536
signal: 17, thread: 140642585937472
signal: 17, thread: 140642591561536
signal: 17, thread: 140642591561536
signal: 17, thread: 140642591561536
^C
*/

void handler(int sig){
    printf("signal: %d, thread: %ld\n", sig, pthread_self());
}

void* makeChild(void *arg){
    printf("makeChild: %ld\n",pthread_self());
    for(int i=0;i<10;i++){
        switch(fork()){
            case -1:
                printf("fork: %s\n",strerror(errno));
                break;
            case 0: // child
                _exit(EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }
    printf("sigmask: %ld\n",pthread_self());
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGCHLD);
    pthread_sigmask(SIG_BLOCK,&set,NULL);
    for(int i=0;i<10;i++){
        switch(fork()){
            case -1:
                printf("fork: %s\n",strerror(errno));
                break;
            case 0: // child
                _exit(EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }
    return NULL;
}

void* waitSignal(void *arg){
    printf("waitSignal: %ld\n",pthread_self());
    while(1){
        pause();
    }
    return NULL;
}

int main(){
    printf("main thread\n: %ld",pthread_self());
    struct sigaction sa;
    sa.sa_handler=handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    sigaction(SIGCHLD,&sa,NULL);


    pthread_t t1, t2;
    pthread_create(&t1,NULL,waitSignal,NULL);
    sleep(1);
    pthread_create(&t2,NULL,makeChild,NULL);

    while(1){
        pause();
    }
}