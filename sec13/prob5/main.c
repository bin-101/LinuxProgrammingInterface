#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024*128 
char buf[BUF_SIZE];
int output_pos=0;

int main(int argc, char *argv[]){
    int output_line=10;
    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1){
        switch (opt){
        case 'n':
            output_line = strtol(optarg, NULL, 10);
            break;
        default:
            fprintf(stderr, "argc error\n");
            exit(1);
        }
    }
    if(output_line==-1){
        fprintf(stderr, "argc error\n");
        exit(1);
    }

    // ファイルサイズを取得する
    if(optind >= argc){
        fprintf(stderr, "argc error\n");
        exit(1);
    }
    char *filename = argv[optind];
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("lseek");
        exit(1);
    }

    // 最後が改行かどうかを確認する。
    // 実験したところ、tail コマンドは最後が改行で終わっている場合を考慮している
    int ends_with_newline = 0;
    if (file_size > 0) {
        char last;
        if (pread(fd, &last, 1, file_size - 1) == 1 && last == '\n') {
            ends_with_newline = 1;
        }
    }

    int line_count=0;
    off_t now_pos=file_size;
    off_t output_pos=-1;
    while(1){
        off_t read_pos=now_pos-BUF_SIZE;
        if(read_pos<0){
            read_pos=0;
        }
        off_t next_read_pos=read_pos-BUF_SIZE;
        if(next_read_pos<0){
            next_read_pos=0;
        }
#ifdef USE_FADVISE
        // 比較実験したところ、あまり効果がなかった。
        int res_advise = posix_fadvise(fd, next_read_pos, now_pos - next_read_pos, POSIX_FADV_WILLNEED);
        if (res_advise != 0) {
            fprintf(stderr, "posix_fadvise: %s\n", strerror(res_advise));
            exit(1);
        }
#endif
        int read_size=pread(fd, buf, sizeof(buf), read_pos);
        if(read_size==-1){
            perror("pread");
            exit(1);
        }
        for(int i=read_size-1; i>=0; i--){
            if(buf[i]=='\n'){
                line_count++;
                if(line_count==output_line+ends_with_newline){
                    output_pos=read_pos+i+1;
                    break;
                }
            }
        }
        if(line_count==output_line+ends_with_newline){
            break;
        }
        if(now_pos==0){
            output_pos=0;
            break;
        }
        now_pos=read_pos;
    }
    // 出力
    off_t now_output_pos=output_pos;
    while (now_output_pos < file_size) {
        int n = pread(fd, buf, BUF_SIZE, now_output_pos);
        if (n == -1) {
            perror("pread");
            exit(1);
        }
        // printf のほうがおそらく高速だが、問題文にI/Oシステムコールを使うように書いてあるため、write を使う
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("write");
            exit(1);
        }
        now_output_pos += n;
    }
    close(fd);
    return 0;
}