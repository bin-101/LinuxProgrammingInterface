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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec36/prob2$ ./a.out sleep 3
  user time: 0
system time: 0
  real time: 3
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec36/prob2$ ./a.out bash -c 'for ((i=0; i<10000000; i++)); do :; done'
  user time: 11
system time: 0
  real time: 11
*/

struct timeval start;

void printTime(){
    struct rusage res_self,res_children;
    if(getrusage(RUSAGE_CHILDREN, &res_children) == -1){
        printf("getrusage: %s\n", strerror(errno));
        exit(1);
    }
    printf("  user time: %ld\n", res_children.ru_utime.tv_sec);
    printf("system time: %ld\n", res_children.ru_stime.tv_sec);

    struct timeval end;
    int res = gettimeofday(&end, NULL);
    if(res == -1){
        printf("gettimeofday: %s\n", strerror(errno));
        exit(1);
    }
    printf("  real time: %ld\n", end.tv_sec - start.tv_sec);
}


int main(int argc, char *argv[]){

    setbuf(stdout, NULL);

    int res = gettimeofday(&start, NULL);
    if(res == -1){
        printf("gettimeofday: %s\n", strerror(errno));
        exit(1);
    }

    switch (fork()){
        case -1:
            printf("fork: %s\n", strerror(errno));
            exit(1);
        case 0:
            execvp(argv[1], &argv[1]);
            printf("execvp: %s\n", strerror(errno));
            exit(1);
        default: // 親プロセス
            break;
    }
    wait(NULL);
    printTime();
    return 0;
}