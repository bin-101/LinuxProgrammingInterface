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
#include <limits.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/mman.h>

// 写経

// #define SHM_KEY 0x1234
#define SHM_NAME "/shm"
#define SEM_KEY 0x5678

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

#define WRITE_SEM 0
#define READ_SEM 1

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

struct shmseg{
    int cnt;
    char buf[BUF_SIZE];
};