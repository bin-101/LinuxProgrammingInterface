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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec34/prob1$ ./a.out
Parent 8844
first_child: 8845
Before: Child 8845 (getpgrep: 8844)
After: Child 8845 (getpgrep: 8845)
Child 8845 loop: 0
Before: Child 8846 (getpgrep: 8844)
After: Child 8846 (getpgrep: 8845)
Child 8846 loop: 0
Child 8845 loop: 1
Child 8846 loop: 1
Child 8845 loop: 2
Parent end
Parent end

catが終了せず、Parent endが2回出力された
*/

int main(){
    setvbuf(stdout,NULL,_IONBF,0);
    printf("Parent %d\n",getpid());

    pid_t first_child;

    for(int i=0;i<2;i++){
        pid_t child;
        switch(child=fork()){
            case -1:
                printf("fork: %s",strerror(errno));
                exit(1);
            case 0:
                printf("Before: Child %d (getpgrep: %d)\n",getpid(),getpgrp());
                int res=setpgid(0,first_child);
                if(res){
                    printf("setpgid: %s\n",strerror(errno));
                    exit(1);
                }
                printf("After: Child %d (getpgrep: %d)\n",getpid(),getpgrp());
                for(int j=0;j<1000;j++){
                    printf("Child %d loop: %d\n",getpid(),j);
                    sleep(1);
                }
                exit(1);
            default:
                if(i==0) first_child=child;
                break;
        }
    }
    printf("first_child: %d\n",first_child);

    sleep(2);

    signal(SIGUSR1,SIG_IGN); // SIGUR1を無視する

    killpg(first_child,SIGUSR1); // 自分のプロセスグループにSIGUSR1を送る。SIGUSR1のディフォルト動作は終了

    printf("Parent end\n"); // これは出力されない
    fprintf(stderr,"Parent end\n"); // これは出力される
}