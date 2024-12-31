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
#include <sys/file.h>

/*
Nをループカウンタとした場合、
30,000以上になると線形に増えないと考えたため、下記のようにコードを書いた。
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob5$ gcc test.c -otest
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob5$ time ./test 0

real    0m0.687s
user    0m0.003s
sys     0m0.684s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob5$ time ./test 10000

real    0m1.338s
user    0m0.003s
sys     0m1.335s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob5$ time ./test 20000

real    0m2.058s
user    0m0.004s
sys     0m2.055s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob5$ time ./test 30000

real    0m2.625s
user    0m0.002s
sys     0m2.623s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob5$ time ./test 40000

real    0m3.365s
user    0m0.004s
sys     0m3.360s
線形に所要時間が延びることを確認できた
*/

int main(int argc,char *argv[]){
    int fd=open("a.txt",O_RDWR);
    if(fd==-1){
        perror("open");
        exit(1);
    }

    struct flock fl;
    fl.l_type=F_WRLCK;
    fl.l_whence=SEEK_SET;
    fl.l_len=1;

    int start=atoi(argv[1]);
    for(int i=0;i<=10000;i++){
        fl.l_start=i*2+start;
        fcntl(fd,F_SETLK,&fl);
    }
}