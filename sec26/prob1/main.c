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

int main(){

    switch(fork()){
        case -1:
            printf("fork: %s",strerror(errno));
            exit(EXIT_FAILURE);
        case 0: // 子プロセス
            sleep(5);
            printf("getppid(): %d\n",getppid());
        default: // 親プロセス
            exit(0);
    }
}

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob1$ ps aux
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root           1  0.0  0.0 168096 12852 ?        Ss   11月10   0:02 /sbin/init splash
~~~~
bin101       983  0.0  0.0  18104 10624 ?        Ss   11月10   0:01 /lib/systemd/systemd --user
bin101       984  0.0  0.0 169928  5712 ?        S    11月10   0:00 (sd-pam)

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob1$ ./a.out
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec26/prob1$ getppid(): 983

systemdのNEWSから引用した：
When running in --user mode systemd will now become a
          subreaper (PR_SET_CHILD_SUBREAPER). This should make the ps
          tree a lot more organized.

*/
