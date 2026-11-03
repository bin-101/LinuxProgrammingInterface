#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../sec59/prob2/socket_library.h"

#define SERVICE "50000"
#define BUF_SIZE 4096

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s server-host command\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sfd = inetConnect(argv[1], SERVICE, SOCK_STREAM);
    if (sfd == -1) {
        perror("inetConnect");
        exit(EXIT_FAILURE);
    }

    size_t commandLen = strlen(argv[2]);
    if (write(sfd, argv[2], commandLen) != (ssize_t) commandLen) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    if (shutdown(sfd, SHUT_WR) == -1) {
        perror("shutdown");
        exit(EXIT_FAILURE);
    }

    char buf[BUF_SIZE];
    ssize_t numRead;
    while ((numRead = read(sfd, buf, BUF_SIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, numRead) != numRead) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    if (numRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(sfd);
    exit(EXIT_SUCCESS);
}
