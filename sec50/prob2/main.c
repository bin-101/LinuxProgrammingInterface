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

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec50/prob2$ ./a.out
0: 0
0: 1
0: 0
0: 97
0: 1
0: 97
*/

void print(void *addr){
    printf("0: %d\n",*(char *)addr);
    char *c=addr;
    *c=1;
    printf("0: %d\n",*(char *)addr);
    if(madvise(addr,100,MADV_DONTNEED)==-1){
        perror("madvise");
        exit(1);
    }
    printf("0: %d\n",*(char *)addr);
}

int main(){
    void *addr=mmap(NULL,100,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0); // 無名非共有マッピング
    print(addr);
    int fd=open("./a100.txt",O_RDONLY);
    addr=mmap(NULL,100,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0); // ファイル非共有マッピング
    print(addr);
}