// cpコマンドを実装
#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec04/prob2$ ./a.out test.txt test_cp.txt
next_data_start: 0
want_read_size: 4096
read_size: 4096
next_data_start: 9998336
want_read_size: 1668
read_size: 1668
*/


int find_next_data(int fd){
    int now_offset=lseek(fd, 0, SEEK_CUR);
    return lseek(fd, now_offset, SEEK_DATA);
}
int find_next_hole(int fd){
    int now_offset=lseek(fd, 0, SEEK_CUR);
    return lseek(fd, now_offset, SEEK_HOLE);
}

int main(int argc, char *argv[]) {
    char* file_name_read=argv[1];
    char* file_name_write=argv[2];
    //ファイルを作成
    int fd_read = open(file_name_read, O_RDONLY);
    int fd_write = open(file_name_write, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);

    while(1){
        int next_data_start=find_next_data(fd_read);
        if(errno==ENXIO){ // https://ja.manpages.org/lseek/2
            break;
        }
        int next_data_end=find_next_hole(fd_read);
        int size=next_data_end-next_data_start;
        char buffer[size];
        lseek(fd_read, next_data_start, SEEK_SET);
        ssize_t read_size=read(fd_read, buffer, size);
        printf("next_data_start: %d\n", next_data_start);
        printf("want_read_size: %d\n", size);
        printf("read_size: %ld\n", read_size);
        lseek(fd_write, next_data_start, SEEK_SET);
        write(fd_write, buffer, read_size);
    }
    close(fd_read);
    close(fd_write);
}