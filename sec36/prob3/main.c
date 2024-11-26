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
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec36/prob3$ ./a.out c
x: -8167783184519509372
elapsed time: 5
RLIMIT_CPU: 0 Success
before sleep
after sleep # すでにリミットを超えているが、CPUを使わない場合は受信しない
signal 24 time: 7
signal 24 time: 7
signal 24 time: 7
signal 24 time: 7
signal 24 time: 7　# 5回一気に受信
signal 24 time: 8
signal 24 time: 9
signal 24 time: 10
signal 24 time: 11 # 1秒ごとに受信
強制終了
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec36/prob3$ echo $?
137 # sigkillを受信したことを確かめた

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec36/prob3$ ./a.out f
RLIMIT_NPROC: 0 Success
fork: -1 Resource temporarily unavailable
*/

void use_utime(long long int loop){
    long long int x=0;
    for(long long int i=0;i<loop;i++){
        if(rand()%2) x+=i*i;
    }
    printf("x: %lld\n", x);
}
time_t start;

void hanlder(int sig){
    printf("signal %d time: %ld\n", sig, time(NULL)-start);
}

int main(int argc, char *argv[]){
    // RLIMIT_CPU
    struct sigaction sa;
    sa.sa_handler=hanlder;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGXCPU, &sa, NULL);
    setbuf(stdout, NULL);

    struct rlimit rlim;
    rlim.rlim_cur=1;
    rlim.rlim_max=10;
    int res;

    if(argv[1][0]=='c'){
        start = time(NULL);
        use_utime(400000000);
        time_t end = time(NULL);
        printf("elapsed time: %ld\n", end-start);
        res=setrlimit(RLIMIT_CPU, &rlim);
        printf("RLIMIT_CPU: %d %s\n", res, strerror(errno));

        printf("before sleep\n");
        sleep(2);
        printf("after sleep\n");

        use_utime(1000000000);
        use_utime(1000000000);   
    }else{
        // RLIMIT_NPROC
        for(int i=0;i<5;i++){
            switch(fork()){
                case -1:
                    printf("fork: %s\n", strerror(errno));
                    exit(1);
                case 0:
                    sleep(10);
                    _exit(0);
            }
        }
        res=setrlimit(RLIMIT_NPROC, &rlim);
        printf("RLIMIT_NPROC: %d %s\n", res, strerror(errno));
        res=fork();
        printf("fork: %d %s\n", res, strerror(errno));
    }
}