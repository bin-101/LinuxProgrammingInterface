#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec24/prob1$ ./a.out
22216
22220
22217
22221
22218
22222
22219
22223
*/

int main(){
    fork();
    fork();
    fork();

    printf("%d\n",getpid());
}