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

int main(int argc,char *argv[]){
    int semid=semget(IPC_PRIVATE,1,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
    printf("semid: %d\n",semid);
    printf("pid: %d\n",getpid());
}
