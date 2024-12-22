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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob1$ gcc main.c
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob1$ ./a.out msync.txt msync_copy.txt

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob1$ ll
合計 36
drwxrwxr-x 2 bin101 bin101  4096 12月 22 19:05 ./
drwxrwxr-x 3 bin101 bin101  4096 12月 22 18:44 ../
-rwxrwxr-x 1 bin101 bin101 16304 12月 22 19:05 a.out*
-rw-rw-r-- 1 bin101 bin101  1394 12月 22 19:05 main.c
-rw-rw-r-- 1 bin101 bin101  3287 12月 22 18:59 msync.txt
-rw------- 1 bin101 bin101  3287 12月 22 19:05 msync_copy.txt # サイズが一致していることを確かめた
*/

int main(int argc,char *argv[]){
    char *input_file=argv[1];
    char *output_file=argv[2];
    int input_fd=open(input_file,O_RDONLY);
    int output_fd=open(output_file,O_RDWR|O_CREAT,S_IWUSR|S_IRUSR);

    struct stat st;
    if(fstat(input_fd,&st)==-1){
        perror("fstat");
        exit(1);
    }
    if(ftruncate(output_fd,st.st_size)==-1){
        perror("ftruncate");
        exit(1);
    }
    void *input_addr=mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,input_fd,0);
    void *output_addr=mmap(NULL,st.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,output_fd,0);

    memcpy(output_addr,input_addr,st.st_size);

    if(msync(output_addr,st.st_size,MS_SYNC)==-1){
        perror("msync");
        exit(1);
    }
}