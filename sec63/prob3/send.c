#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define MAX_TEXT 1024

struct message {
    long mtype;
    char mtext[MAX_TEXT];
};

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s msqid message\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct message msg;
    msg.mtype = 1;

    size_t msgLen = strlen(argv[2]) + 1;
    if (msgLen > MAX_TEXT) {
        fprintf(stderr, "message is too long\n");
        exit(EXIT_FAILURE);
    }
    memcpy(msg.mtext, argv[2], msgLen);

    if (msgsnd(atoi(argv[1]), &msg, msgLen, 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
