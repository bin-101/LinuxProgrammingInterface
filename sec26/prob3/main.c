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

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob3$ ./a.out -1
Child started with PID = 21525
Child exited, status=255

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob3$ ./a.out 90
Child started with PID = 21541
Child exited, status=90

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob3$ ./a.out &
[1] 21783
Child started with PID = 21785
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob3$ kill 21785
Child killed by signal 15 (Terminated)
[1]+  終了                  ./a.out

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob3$ ./a.out &
[1] 22141
Child started with PID = 22143
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob3$ kill -SIGSTOP 22143
Child stopped by signal 19 (Stopped (signal))
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob3$ kill -SIGCONT 22143
Child continued

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob3$ kill -SIGABRT 22379
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob3$ Child killed by signal 6 (Aborted)
 (core dumped)
[1]+  終了                  ./a.out
*/

// printWaitStatusを再現
void printSigInfo(const char *msg, siginfo_t *infop){
    if(msg!=NULL){
        printf("%s", msg);
    }
    if(infop->si_code==CLD_EXITED){
        printf("Child exited, status=%d\n", infop->si_status);
    }else if(infop->si_code==CLD_KILLED || infop->si_code==CLD_DUMPED){
        printf("Child killed by signal %d (%s)\n", infop->si_status, strsignal(infop->si_status));
        if(infop->si_code==CLD_DUMPED){ // https://ja.manpages.org/waitid/2
            printf(" (core dumped)");
        }
    }else if(infop->si_code==CLD_STOPPED){
        printf("Child stopped by signal %d (%s)\n", infop->si_status, strsignal(infop->si_status));
    }else if(infop->si_code==CLD_CONTINUED){
        printf("Child continued\n");
    }else{
        printf("CLD_TRAPPED?\n");
    }
}

int main(int argc, char *argv[]){
    switch(fork()){
        case 0:
            printf("Child started with PID = %ld\n", (long)getpid());
            if(argc>1){
                exit(atoi(argv[1]));
            }else{
                while(1){
                    pause();
                }
            }
            exit(EXIT_FAILURE); // ここには到達しない
        default:
            while(1){
                siginfo_t infop;
                int res=waitid(P_ALL, -123, &infop, WEXITED|WSTOPPED|WCONTINUED);
                if(res==-1){
                    printf("waitid: errno=%s\n", strerror(errno));
                }
                printSigInfo(NULL, &infop);
                if(infop.si_code==CLD_EXITED || infop.si_code==CLD_KILLED || infop.si_code==CLD_DUMPED){
                    break;
                }
            }
    }
}