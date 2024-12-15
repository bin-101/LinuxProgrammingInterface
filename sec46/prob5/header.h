#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <limits.h>
#include <stddef.h>

// 例46-7 写経

#define SERVER_KEY 0x1aaaaaa1

struct requestMsg{
    long mtype;
    int clientId;
    char pathname[PATH_MAX];
};

#define REQ_MSG_SIZE (offsetof(struct requestMsg,pathname)- offsetof(struct requestMsg,clientId)+PATH_MAX)

#define RESP_MSG_SIZE 8192

struct responseMsg{
    long mtype;
    char data[RESP_MSG_SIZE];
};

#define RESP_MT_FAILURE 1
#define RESP_MT_DATA 2
#define RESP_MT_END 3