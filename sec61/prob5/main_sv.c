#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../../sec59/prob2/socket_library.h"

#define SERVICE "50000"
#define BUF_SIZE 4096


/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob5$ gcc main_cl.c -oclient
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob5$ gcc main_sv.c -oserver
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob5$ ./server &
[1] 113422
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob5$ ./client localhost 'echo hello; uname -s'
hello
Linux
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob5$ ./client localhost 'exit'
*/

void grimReaper(int sig)
{
    (void) sig;
    int savedErrno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;

    errno = savedErrno;
}

void handleRequest(int cfd)
{
    char command[BUF_SIZE];
    size_t total = 0;
    ssize_t numRead;

    while (total < BUF_SIZE - 1 &&
           (numRead = read(cfd, command + total,
                           BUF_SIZE - 1 - total)) > 0)
        total += numRead;

    if (numRead == -1) {
        perror("read");
        _exit(EXIT_FAILURE);
    }

    command[total] = '\0';

    if (dup2(cfd, STDOUT_FILENO) == -1 ||
        dup2(cfd, STDERR_FILENO) == -1)
        _exit(EXIT_FAILURE);

    close(cfd);
    system(command);
}

int main(void)
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    int lfd = inetListen(SERVICE, 10, NULL);
    if (lfd == -1) {
        perror("inetListen");
        exit(EXIT_FAILURE);
    }

    while (TRUE) {
        int cfd = accept(lfd, NULL, NULL);
        if (cfd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        switch (fork()) {
        case -1:
            perror("fork");
            close(cfd);
            break;
        case 0:
            close(lfd);
            signal(SIGCHLD, SIG_DFL);
            handleRequest(cfd);
            _exit(EXIT_SUCCESS);
        default:
            close(cfd);
            break;
        }
    }
}
