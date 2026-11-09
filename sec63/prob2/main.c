#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ nc 127.0.0.1 50000
hello tcp
hello tcp
^C
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ nc -u 127.0.0.1 50000
hello udp
hello udp
*/

#define PORT 50000
#define BUF_SIZE 4096

void grimReaper(int sig)
{
    (void) sig;
    int savedErrno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;

    errno = savedErrno;
}

int createSocket(int type)
{
    int sfd = socket(AF_INET, type, 0);
    if (sfd == -1)
        return -1;

    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        close(sfd);
        return -1;
    }

    return sfd;
}

void handleTcpRequest(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t numRead;

    while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
        if (write(cfd, buf, numRead) != numRead)
            _exit(EXIT_FAILURE);
    }
}

void handleUdpRequest(int sfd)
{
    char buf[BUF_SIZE];
    struct sockaddr_storage clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    ssize_t numRead = recvfrom(sfd, buf, BUF_SIZE, 0,
                               (struct sockaddr *) &clientAddr, &addrLen);
    if (numRead == -1) {
        perror("recvfrom");
        return;
    }

    if (sendto(sfd, buf, numRead, 0,
               (struct sockaddr *) &clientAddr, addrLen) != numRead)
        perror("sendto");
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

    int tcpFd = createSocket(SOCK_STREAM);
    int udpFd = createSocket(SOCK_DGRAM);
    if (tcpFd == -1 || udpFd == -1) {
        perror("createSocket");
        exit(EXIT_FAILURE);
    }

    if (listen(tcpFd, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // POLLIN: 読み取り可能
    struct pollfd fds[2] = {
        { tcpFd, POLLIN, 0 },
        { udpFd, POLLIN, 0 }
    };

    for (;;) {
        if (poll(fds, 2, -1) == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) {
            int cfd = accept(tcpFd, NULL, NULL);
            if (cfd == -1) {
                perror("accept");
                continue;
            }

            switch (fork()) {
            case -1:
                perror("fork");
                close(cfd);
                break;
            case 0:
                close(tcpFd);
                close(udpFd);
                handleTcpRequest(cfd);
                close(cfd);
                _exit(EXIT_SUCCESS);
            default:
                close(cfd);
                break;
            }
        }

        if (fds[1].revents & POLLIN)
            handleUdpRequest(udpFd);
    }
}
