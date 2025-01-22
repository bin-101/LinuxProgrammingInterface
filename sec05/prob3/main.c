#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec05/prob3$ ./a.out f1 1000000 & ./a.out f1 1000000
[1] 121908
[1]+  終了                  ./a.out f1 1000000
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec05/prob3$ ./a.out f2 1000000 x & ./a.out f2 1000000 x
[1] 121924
[1]+  終了                  ./a.out f2 1000000 x
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec05/prob3$ ls -l
合計 3312
-rw-rw-r-- 1 bin101 bin101       0  1月 23 00:29 AC
-rwxrwxr-x 1 bin101 bin101   16224  1月 23 00:30 a.out
-rwxrwxr-x 1 bin101 bin101 1365959  1月 23 00:30 f1
-rwxrwxr-x 1 bin101 bin101 2000000  1月 23 00:30 f2
-rw-rw-r-- 1 bin101 bin101    1762  1月 23 00:30 main.c

理由
- O_APPENDを指定すると、末尾への移動と書き込みをアトミックに行うため。
- O_APPENDを指定しない場合、書き込みがアトミックではないため、書き込み位置がずれる。
*/

int main(int argc, char *argv[])
{
    char *filename = argv[1];
    int num_bytes = atoi(argv[2]);
    bool is_append = false;
    if(argc >= 4){
        if(strcmp(argv[3], "x") == 0) is_append = true;
    }
    char buffer[num_bytes];
    for (int i=0; i<num_bytes; i++){
        buffer[i]='a';
    }

    int fd;
    if(is_append){
        fd=open(filename, O_WRONLY | O_APPEND | O_CREAT, 0777);
    }else{
        fd=open(filename, O_WRONLY | O_CREAT, 0777);
    }
    for(int i=0;i<num_bytes;i++){
        if(is_append==false) lseek(fd,0,SEEK_END);
        write(fd,buffer,1);
    }
    close(fd);

    return 0;
}