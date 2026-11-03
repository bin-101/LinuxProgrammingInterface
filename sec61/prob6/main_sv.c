#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../../sec59/prob2/socket_library.h"

#define SERVICE "50000"
#define BUF_SIZE 4096
#define COOKIE_LEN 32

void grimReaper(int sig)
{
    (void) sig;
    int savedErrno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;

    errno = savedErrno;
}

void receiveData(int normalFd, int highFd)
{
    struct pollfd fds[2] = {
        { highFd, POLLIN, 0 },
        { normalFd, POLLIN, 0 }
    };
    const char *label[2] = { "HIGH", "normal" };
    int openFds = 2;
    char buf[BUF_SIZE];

    while (openFds > 0) {
        // 第3引数のタイムアウトを-1にして、無限に待機する
        if (poll(fds, 2, -1) == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");
            return;
        }

        for (int i = 0; i < 2; i++) {
            if (fds[i].fd == -1)
                continue;

            if (fds[i].revents & POLLIN) {
                ssize_t numRead = read(fds[i].fd, buf, BUF_SIZE);
                if (numRead > 0) {
                    dprintf(STDOUT_FILENO, "[%s] ", label[i]);
                    write(STDOUT_FILENO, buf, numRead);
                    continue;
                }
            }
            // 読み取りができない場合、ファイルディスクリプタを閉じる
            // POLLHUP: 相手が接続を閉じた場合
            // POLLERR: エラーが発生した場合
            // POLLNVAL: 無効なファイルディスクリプタの場合
            if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                close(fds[i].fd);
                fds[i].fd = -1;
                openFds--;
            }
        }
    }
}

void handleRequest(int normalFd, struct sockaddr *peerAddr,
                   socklen_t peerAddrLen)
{
    char handshake[128];
    char port[16];
    char cookie[COOKIE_LEN + 1];
    char host[NI_MAXHOST];

    if (readLine(normalFd, handshake, sizeof(handshake)) <= 0 ||
        // handshakeのフォーマットは "port cookie" であることを期待する
        sscanf(handshake, "%15s %32s", port, cookie) != 2) {
        fprintf(stderr, "invalid handshake\n");
        return;
    }

    int error = getnameinfo(peerAddr, peerAddrLen, host, sizeof(host),
                            NULL, 0, NI_NUMERICHOST);
    if (error != 0) {
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(error));
        return;
    }

    int highFd = inetConnect(host, port, SOCK_STREAM);
    if (highFd == -1) {
        perror("inetConnect");
        return;
    }

    // cookieを送信する
    if (dprintf(highFd, "%s\n", cookie) == -1) {
        perror("write cookie");
        close(highFd);
        return;
    }

    receiveData(normalFd, highFd);
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
        struct sockaddr_storage peerAddr;
        socklen_t peerAddrLen = sizeof(peerAddr);
        int normalFd = accept(lfd, (struct sockaddr *) &peerAddr,
                              &peerAddrLen);
        if (normalFd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        switch (fork()) {
        case -1:
            perror("fork");
            close(normalFd);
            break;
        case 0:
            close(lfd);
            handleRequest(normalFd, (struct sockaddr *) &peerAddr,
                          peerAddrLen);
            close(normalFd);
            _exit(EXIT_SUCCESS);
        default:
            close(normalFd);
            break;
        }
    }
}
