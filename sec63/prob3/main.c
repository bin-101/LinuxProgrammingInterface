#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_TEXT 1024

struct message {
    long mtype;
    char mtext[MAX_TEXT];
};

volatile sig_atomic_t stop = 0;

void stopHandler(int sig)
{
    (void) sig;
    stop = 1;
}

void receiveMessages(int msqid, int pipeFd)
{
    struct message msg;
    char output[MAX_TEXT + 100];

    for (;;) {
        ssize_t msgLen = msgrcv(msqid, &msg, MAX_TEXT, 0, 0);
        if (msgLen == -1) {
            if (errno == EIDRM || errno == EINVAL)
                break;
            perror("msgrcv");
            break;
        }

        int outputLen = snprintf(output, sizeof(output),
                                 "[queue] type=%ld: %.*s\n",
                                 msg.mtype, (int) msgLen, msg.mtext);
        if (write(pipeFd, output, outputLen) != outputLen)
            break;
    }
}

int main(void)
{
    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    int pipeFd[2];
    if (pipe(pipeFd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t childPid = fork();
    if (childPid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (childPid == 0) {
        close(pipeFd[0]);
        receiveMessages(msqid, pipeFd[1]);
        close(pipeFd[1]);
        _exit(EXIT_SUCCESS);
    }

    close(pipeFd[1]);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = stopHandler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    printf("msqid: %d\n", msqid);
    fflush(stdout);

    char buf[MAX_TEXT];
    while (!stop) {
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(STDIN_FILENO, &readFds);
        FD_SET(pipeFd[0], &readFds);

        int maxFd = pipeFd[0] > STDIN_FILENO ? pipeFd[0] : STDIN_FILENO;
        int ready = select(maxFd + 1, &readFds, NULL, NULL, NULL);
        if (ready == -1) {
            if (errno == EINTR)
                continue;
            perror("select");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readFds)) {
            ssize_t numRead = read(STDIN_FILENO, buf, sizeof(buf));
            if (numRead <= 0)
                break;
            write(STDOUT_FILENO, "[terminal] ", 11);
            write(STDOUT_FILENO, buf, numRead);
        }

        if (FD_ISSET(pipeFd[0], &readFds)) {
            ssize_t numRead = read(pipeFd[0], buf, sizeof(buf));
            if (numRead <= 0)
                break;
            write(STDOUT_FILENO, buf, numRead);
        }
    }

    msgctl(msqid, IPC_RMID, NULL);
    close(pipeFd[0]);
    waitpid(childPid, NULL, 0);
    exit(EXIT_SUCCESS);
}
