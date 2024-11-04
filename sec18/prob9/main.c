#define _GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> // PATH_MAXが定義されている
#include <dirent.h>
#include <ftw.h>

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec18/prob9$ time ./a.out
now_path: /home/bin101/code/LinuxProgrammingInterface/sec18/prob9

real    0m2.104s
user    0m0.114s
sys     0m1.991s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec18/prob9$ time ./a.out f
now_path: /home/bin101/code/LinuxProgrammingInterface/sec18/prob9

real    0m1.293s
user    0m0.118s
sys     0m1.176s

fchdirの方が速い
- chdirはユーザー空間からカーネル空間にパス名を渡す必要がある
- chdirはi-node番号を取得する必要がある
*/

int main(int argc, char *argv[]){
    int loop=1000000;
    char path[]="/home/bin101/code/LinuxProgrammingInterface/sec13";
    char *now_path=getcwd(NULL,0);
    printf("now_path: %s\n",now_path);
    if(argc>=2 && argv[1][0]=='f'){
        int fd=open(".",O_RDONLY);
        while(loop--){
            chdir(path);
            fchdir(fd);
        }
    }else{
        while(loop--){
            chdir(path);
            chdir(now_path);
        }
    }
}