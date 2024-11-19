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
Parent PID=45937
Child (PID=45938) exiting
Signal 17 (Child exited) received
  45937 pts/0    00:00:00 a.out
  45938 pts/0    00:00:00 a.out <defunct>
Signal 17 (Child exited) received
After sending SIGKILL to zombie (PID=45938):
  45937 pts/0    00:00:00 a.out
  45938 pts/0    00:00:00 a.out <defunct>
Signal 17 (Child exited) received

なぜ、signal 17を3回受信するのかはわからず。system(cmd)を増やすとsignal 17を受信する回数も同じ数だけ増えた。
*/

#define CMD_SIZE 200

void handler(int sig){
    printf("Signal %d (%s) received\n",sig,strsignal(sig));
}

int main(int argc,char *argv[]){


    setbuf(stdout,NULL);

    printf("Parent PID=%ld\n",(long)getpid());

    pid_t childPid;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    sa.sa_handler=handler;
    sigaction(SIGCHLD,&sa,NULL);

    switch(childPid=fork()){
        case -1:
            printf("fork: %s\n",strerror(errno));
        case 0:
            printf("Child (PID=%ld) exiting\n",(long)getpid());
            _exit(EXIT_SUCCESS);
        default:
            pause(); // SIGCHILDを受信するまで待つ
            char cmd[CMD_SIZE];
            snprintf(cmd,CMD_SIZE,"ps | grep %s",basename(argv[0]));
            system(cmd);

            if(kill(childPid,SIGKILL)==-1){
                printf("kill: %s\n",strerror(errno));
            }
            sleep(3); // ここを消すのはおそらく無理？
            printf("After sending SIGKILL to zombie (PID=%ld):\n",(long)childPid);
            system(cmd);
    }
}