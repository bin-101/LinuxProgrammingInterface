#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

/*
pipe:
- pipefd[0]は読み取り口、pipefd[1]は書き込み口
- バイトストリーム
*/

int myPipe(int pipefd[2])
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd) == -1)
        return -1;

    /* pipefd[0]を読み取り口、pipefd[1]を書き込み口にする。 */
    if (shutdown(pipefd[0], SHUT_WR) == -1 ||
        shutdown(pipefd[1], SHUT_RD) == -1)
        return -1;

    return 0;
}
