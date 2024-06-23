// vfork後に、親プロセスと子プロセスがファイルディスクリプタを共有していないことを示せ
// p552の例24-4を参考にした

#define _GNU_SOURCE
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *buffer="before vfork\n";
    write(STDOUT_FILENO, buffer, strlen(buffer));

    pid_t child_pid;
    switch (child_pid=vfork()){
    case -1:
        perror("vfork");
        exit(EXIT_FAILURE);
    case 0: //子プロセス
        buffer="child process\n";
        write(STDOUT_FILENO, buffer, strlen(buffer));
        close(STDOUT_FILENO);
        write(STDOUT_FILENO, buffer, strlen(buffer));
        _exit(EXIT_SUCCESS);
    default: //親プロセス
        int status;
        wait(&status);
        printf("status: %d\n", status);
        buffer="parent process\n";
        write(STDOUT_FILENO, buffer, strlen(buffer));
        exit(EXIT_SUCCESS);
    }

    return 0;
}