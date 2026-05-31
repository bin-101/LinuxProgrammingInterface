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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec24/prob3$ ./a.out
parent process: wait(134)
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec24/prob3$ ls -lh
合計 156K
-rwxrwxr-x 1 bin101 bin101  17K  5月 31 21:46 a.out
-rw------- 1 bin101 bin101 304K  5月 31 21:49 core.106248
-rw-rw-r-- 1 bin101 bin101 1.1K  5月 31 21:45 main.c
*/

int main(int argc, char *argv[])
{
    pid_t child_pid;
    switch (child_pid=vfork()){
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: //子プロセス
            // SIGABRTのブロックを解除
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGABRT);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);

            // SIGABRTのハンドラをデフォルトに設定
            struct sigaction sa;
            sa.sa_handler = SIG_DFL;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(SIGABRT, &sa, NULL);

            abort();
            printf("this line must not be printed\n");
        default: //親プロセス
            int status;
            wait(&status);
            printf("parent process: wait(%d)\n", status);
    }
    return 0;
}