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
ftok()
    key_t ftok(char *pathname, int proj)
        projの下位8bit
        デバイス番号の下位8bit (マイナデバイス番号)
        i-node番号の下位16bit

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec45/prob1$ ./a.out
ftok: 12075252 (proj: 0x12)
device ID: 10307 (07)
i-node: 19e5252 (5252)
*/

int main(){
    printf("ftok: %x\n",ftok("./test.txt",0x12));

    struct stat st;
    char path[]="./test.txt";
    int res=stat(path,&st);
    if(res==-1){
        printf("%s\n",strerror(errno));
        return 1;
    }
    printf("device ID: %lx\n",st.st_dev);
    printf("i-node: %lx\n,",st.st_ino);
}