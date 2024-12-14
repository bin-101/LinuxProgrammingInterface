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

int mask(int x){
    return (1<<x)-1;
}

/*
参考: 45-1の結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec45/prob1$ ./a.out
ftok: 12075252 (proj: 0x12)
device ID: 10307 (07)
i-node: 19e5252 (5252)

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec45/prob2$ ./a.out
myftok: 12075296
ftok: 12075296
*/

key_t myftok(char *pathname, int proj){
    struct stat st;
    if(stat(pathname,&st)==-1){
        return -1;
    }

    key_t _proj=proj&mask(8);
    key_t _device=st.st_dev&mask(8);
    key_t _ino=st.st_ino&mask(16);
    return (_proj<<24) | (_device<<16) | _ino;
}

int main(){
    printf("myftok: %x\n",myftok("./test.txt",0x12));
    printf("ftok: %x\n",ftok("./test.txt",0x12));
}