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
#include <sys/resource.h>

/*
int getrusage(int who, struct rusage *res_usage)
    - 返り値: 成功時0 エラー時-1

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec36/prob1$ ./a.out
children
    user time: 0
  system time: 0
self
    user time: 0
  system time: 0
x: 5428715621627836845
signal: 10
children
    user time: 0
  system time: 0
self
    user time: 0
  system time: 0
children
    user time: 12
  system time: 0
self
    user time: 0
  system time: 0
*/

void printTime(){
    struct rusage res_self,res_children;
    if(getrusage(RUSAGE_CHILDREN, &res_children) == -1){
        printf("getrusage: %s\n", strerror(errno));
        exit(1);
    }
    if(getrusage(RUSAGE_SELF, &res_self) == -1){
        printf("getrusage: %s\n", strerror(errno));
        exit(1);
    }
    printf("children\n");
    printf("    user time: %ld\n", res_children.ru_utime.tv_sec);
    printf("  system time: %ld\n", res_children.ru_stime.tv_sec);
    printf("self\n");
    printf("    user time: %ld\n", res_self.ru_utime.tv_sec);
    printf("  system time: %ld\n", res_self.ru_stime.tv_sec);
}

// sleepではユーザーCPU時間、システムCPU時間は増えないため、適当な計算を行う
void use_utime(){
    long long int x=0;
    for(long long int i=0;i<1000000000;i++){
        if(rand()%2) x+=i*i;
    }
    printf("x: %lld\n", x);
}

void handler(int sig){
    printf("signal: %d\n", sig);
}

int main(){
    printTime();

    if(fork()==0){
        use_utime();
        kill(getppid(), SIGUSR1);
        exit(0);
    }
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    pause(); // pause()によって、ユーザーCPU時間、システムCPU時間は増えない

    printTime();

    wait(NULL);

    printTime();
}