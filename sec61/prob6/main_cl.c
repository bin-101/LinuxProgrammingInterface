#define _GNU_SOURCE
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <unistd.h>
#include "../../sec59/prob2/socket_library.h"

#define SERVICE "50000"
#define BUF_SIZE 4096
#define COOKIE_BYTES 16
#define COOKIE_LEN (COOKIE_BYTES * 2)

// ランダムなcookieを生成する関数
void makeCookie(char cookie[COOKIE_LEN + 1])
{
    unsigned char randomBytes[COOKIE_BYTES];

    if (getrandom(randomBytes, sizeof(randomBytes), 0) !=
        (ssize_t) sizeof(randomBytes)) {
        perror("getrandom");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < COOKIE_BYTES; i++)
        sprintf(cookie + i * 2, "%02x", randomBytes[i]);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s server-host\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // 高速通信のためのリスニングソケットを作成する
    // "0"を指定することで、エファメラルポートをOSに割り当ててもらう
    int highListenFd = inetListen("0", 1, NULL);
    if (highListenFd == -1) {
        perror("inetListen");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_storage addr;
    socklen_t addrLen = sizeof(addr);
    if (getsockname(highListenFd, (struct sockaddr *) &addr, &addrLen) == -1) {
        perror("getsockname");
        exit(EXIT_FAILURE);
    }

    // エファメラルポート番号を取得する
    char port[NI_MAXSERV];
    int error = getnameinfo((struct sockaddr *) &addr, addrLen,
                            NULL, 0, port, sizeof(port), NI_NUMERICSERV);
    if (error != 0) {
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(error));
        exit(EXIT_FAILURE);
    }

    // 通常通信のためのソケットを作成する
    int normalFd = inetConnect(argv[1], SERVICE, SOCK_STREAM);
    if (normalFd == -1) {
        perror("inetConnect");
        exit(EXIT_FAILURE);
    }

    // ランダムなcookieを生成して、サーバに送信する
    char cookie[COOKIE_LEN + 1];
    makeCookie(cookie);
    if (dprintf(normalFd, "%s %s\n", port, cookie) == -1) {
        perror("write handshake");
        exit(EXIT_FAILURE);
    }

    int highFd;
    for (;;) {
        highFd = accept(highListenFd, NULL, NULL);
        if (highFd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // サーバから送信されたcookieを受信して、送信したcookieと比較する
        char receivedCookie[COOKIE_LEN + 2];
        if (readLine(highFd, receivedCookie, sizeof(receivedCookie)) > 0) {
            receivedCookie[strcspn(receivedCookie, "\r\n")] = '\0';
            if (strcmp(receivedCookie, cookie) == 0)
                break;
        }

        close(highFd);
    }
    close(highListenFd);

    // 標準入力からのデータを読み取り、通常通信または高速通信のソケットに送信する
    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        int fd = normalFd;
        char *data = buf;

        if (buf[0] == '!') {
            fd = highFd;
            data++;
        }

        size_t len = strlen(data);
        if (write(fd, data, len) != (ssize_t) len) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    shutdown(normalFd, SHUT_WR);
    shutdown(highFd, SHUT_WR);
    close(normalFd);
    close(highFd);
    exit(EXIT_SUCCESS);
}
