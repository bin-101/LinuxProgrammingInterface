#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

int main(int argc, char *argv[]) {

    //ファイルを作成
    int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);

    const int size=2;
    // バッファを作成
    char buffer[]={'a','\0'};
    // バッファを書き込み
    write(fd, buffer, size);
    lseek(fd, 10000000, SEEK_END);
    // 現在のファイルオフセットを出力
    printf("offset: %ld\n", lseek(fd, 0, SEEK_CUR)); // 10000002
    char buffer2[]={'b','\0'};
    write(fd, buffer2, size);
    printf("offset: %ld\n", lseek(fd, 0, SEEK_CUR)); // 10000004

    close(fd);

    {
        int fd = open("test_full.txt", O_RDONLY);
        //ファイルホールができているか確認
        lseek(fd, 0, SEEK_SET);
        printf("offset: %ld\n", lseek(fd, 0, SEEK_CUR)); // 0
        int now_offset=lseek(fd, 0, SEEK_HOLE);
        printf("offset: %d\n", now_offset); // 4096
        lseek(fd, now_offset, SEEK_DATA);
        printf("offset: %ld\n", lseek(fd, 0, SEEK_CUR));  //9998336
        printf("errno: %d\n", errno); // 0
        close(fd);
    }
    char buffer3[1000000];
    for(int i=0;i<1000000;i++){
        buffer3[i]='c';
    }
    buffer3[1000000-1]='\0';
    fd = open("test_full.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    write(fd, buffer3, 1000000);
    close(fd);
}