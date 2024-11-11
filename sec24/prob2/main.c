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
ファイルディスクリプタテーブルはカーネルがプロセス単位に管理するため、vfork後でも親プロセスは子プロセスの影響を受けない

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec24/prob2$ ./a.out
before vfork
child process
child process: Bad file descriptor
parent process: wait(0) // 出力できている
*/

int main(int argc, char *argv[])
{
    char *buffer="before vfork\n";
    write(STDOUT_FILENO, buffer, strlen(buffer));

    pid_t child_pid;
    switch (child_pid=vfork()){
    case -1:
        printf("fork: %s",strerror(errno));
        exit(EXIT_FAILURE);
    case 0: //子プロセス
        buffer="child process\n";
        write(STDOUT_FILENO, buffer, strlen(buffer));
        close(STDOUT_FILENO);
        if(write(STDOUT_FILENO, buffer, strlen(buffer))==-1){
            char s[100];
            snprintf(s, 100, "child process: %s\n", strerror(errno));
            write(STDERR_FILENO, s, strlen(s));
        }
        _exit(EXIT_SUCCESS);
    default: //親プロセス
        int status;
        wait(&status);
        char s[100];
        snprintf(s, 100, "parent process: wait(%d)\n", status);
        write(STDOUT_FILENO, s, strlen(s));
        exit(EXIT_SUCCESS);
    }

    return 0;
}