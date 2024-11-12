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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec25/prob1$ ./a.out
exit(-1)
parent process: wait(65280) // 255 << 8
WEXITSTATUS: 255

abort()
parent process: wait(134) 128+6
WTERMSIG: 6
WCOREDUMP: 128

分かったこと
- exit()は下位8ビットを終了ステータスとみなす
- wait(&status)
    - 1bit目~7bit目: 終了する原因となったシグナルの番号
    - 8bit目: コアダンプしたかどうか
        - プログラムを実行した際、ulimit -cを実行して0だったが、bitが立っていた
    - 9bit目~16bit目: 終了ステータス
*/

int main(int argc, char *argv[])
{
    write(STDOUT_FILENO, "exit(-1)\n", strlen("exit(-1)\n"));

    pid_t child_pid;
    switch (child_pid=fork()){
    case -1:
        printf("fork: %s",strerror(errno));
        exit(EXIT_FAILURE);
    case 0: //子プロセス
        exit(-1);
    default: //親プロセス
        int status;
        wait(&status);
        char s[100];
        snprintf(s, 100, "parent process: wait(%d)\n", status);
        write(STDOUT_FILENO, s, strlen(s));
        snprintf(s, 100, "WEXITSTATUS: %d\n", WEXITSTATUS(status)); //終了コードを得る ((status) & 0xff00) >> 8
        write(STDOUT_FILENO, s, strlen(s));
    }

    write(STDOUT_FILENO, "\nabort()\n", strlen("\nabort()\n"));
    switch (child_pid=fork()){
    case -1:
        printf("fork: %s",strerror(errno));
        exit(EXIT_FAILURE);
    case 0: //子プロセス
        abort();
    default: //親プロセス
        int status;
        wait(&status);
        char s[100];
        snprintf(s, 100, "parent process: wait(%d)\n", status);
        write(STDOUT_FILENO, s, strlen(s));
        snprintf(s, 100, "WTERMSIG: %d\n", WTERMSIG(status)); //終了シグナルを得る ((status) & 0x7f)
        write(STDOUT_FILENO, s, strlen(s));
        snprintf(s, 100, "WCOREDUMP: %d\n", WCOREDUMP(status)); //コアダンプがあるかどうか ((status) & __WCOREFLAG) __WCOREFLAG: 0x80(128)
        write(STDOUT_FILENO, s, strlen(s));
        exit(EXIT_SUCCESS);
    }

    return 0;
}