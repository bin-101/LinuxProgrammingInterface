#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
./a.out f1 1000000 & ./a.out f1 1000000
- 前半のコマンドがバックグラウンドで実行されている？
- 1094209文字(ランダム)書き込まれた

./a.out f1 1000000 x & ./a.out f1 1000000 x
- 2000000文字

TODO: prob2とprob3の考察
*/

int main(int argc, char *argv[])
{
    char *filename = argv[1];
    int num_bytes = atoi(argv[2]);
    bool is_append = false;
    if(argc >= 4){
        if(strcmp(argv[3], "x") == 0) is_append = true;
    }
    char buffer[num_bytes+1];
    for (int i=0; i<num_bytes; i++){
        buffer[i]='a';
    }
    buffer[num_bytes]='\0';

    if(is_append){
        int fd=open(filename, O_WRONLY | O_APPEND | O_CREAT, 0777);
        write(fd, buffer, num_bytes);
        close(fd);
    }else{
        int fd=open(filename, O_WRONLY | O_CREAT, 0777);
        lseek(fd, 0, SEEK_END);
        write(fd, buffer, num_bytes);
        close(fd);
    }

    return 0;
}