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
- 1020480文字(ランダム)書き込まれた
offset=0
offset=20480

./a.out f2 1000000 x & ./a.out f2 1000000 x
- 2000000文字

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
        int offset=lseek(fd, 0, SEEK_END);
        printf("offset=%d\n", offset);
        write(fd, buffer, num_bytes);
        close(fd);
    }

    return 0;
}