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
#include <sys/time.h>
#include <fcntl.h>


/*
バンド幅について：
1 ~ 512: ブロックサイズとほぼ比例
512 -> 2048: 大きくなるが、比例ではない
2048 -> 65536: 逆に小さくなる？
65536 -> 16777216: ほぼ一定

65536付近で頭打ちになる原因：
パイプ容量が65536バイトであるため
printf("%d\n", fcntl(pipefd[0], F_GETPIPE_SZ)); // 65536

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 1
Elapsed time: 9.491988 seconds
bandwidth: 1.685632 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 2
Elapsed time: 4.798909 seconds
bandwidth: 3.334091 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 4
Elapsed time: 2.403217 seconds
bandwidth: 6.657743 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 16
Elapsed time: 0.607929 seconds
bandwidth: 26.318863 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 256
Elapsed time: 0.056011 seconds
bandwidth: 285.658174 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 512
Elapsed time: 0.033041 seconds
bandwidth: 484.246845 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 2048
Elapsed time: 0.020286 seconds
bandwidth: 788.721286 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 65536
Elapsed time: 0.027651 seconds
bandwidth: 578.640917 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 1048576
Elapsed time: 0.029278 seconds
bandwidth: 546.485416 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec43/prob1$ ./a.out 16777216
Elapsed time: 0.027764 seconds
bandwidth: 576.285838 MB/s
*/

const int total_size=(1U<<24);
struct timeval start,end;
int pipefd[2];

void write_to_pipe(int fd,char *buf,int block_size){
    int total_size_written=0;
    while(total_size_written<total_size){
        ssize_t size_write=write(fd,buf+total_size_written,block_size);
        if(size_write==-1){
            perror("write");
            exit(1);
        }
        total_size_written+=size_write;
    }
}

void read_from_pipe(int fd,char *buf,int block_size){
    int total_size_read=0;
    while(total_size_read<total_size){
        ssize_t size_read=read(fd,buf+total_size_read,block_size);
        if(size_read==-1){
            perror("read");
            exit(1);
        }
        total_size_read+=size_read;
    }
}

int main(int argc,char *argv[]){
    if(pipe(pipefd)==-1){
        perror("pipe");
        exit(1);
    }
    printf("%d\n", fcntl(pipefd[0], F_GETPIPE_SZ));
    if(argc!=2){
        fprintf(stderr,"Usage: %s <block_size>\n",argv[0]);
        exit(1);
    }
    size_t block_size = strtoull(argv[1], NULL, 10);
    char *buf_write=malloc(total_size);
    if(buf_write==NULL){
        perror("malloc");
        exit(1);
    }
    char *buf_read=malloc(total_size);
    if(buf_read==NULL){
        perror("malloc");
        exit(1);
    }

    // 時間計測開始
    // fork後に行いたいが妥協
    gettimeofday(&start,NULL);
    switch(fork()){
        case -1:
            perror("fork");
            exit(1);
        case 0: // 子プロセス
            // 書き込み
            write_to_pipe(pipefd[1],buf_write,block_size);
            exit(0);
        default: // 親プロセス
            // 読み込み
            read_from_pipe(pipefd[0],buf_read,block_size);
            // 時間計測終了
            gettimeofday(&end,NULL);
            // 経過時間を計算
            double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            printf("Elapsed time: %f seconds\n", elapsed_time);
            printf("bandwidth: %f MB/s\n", (double)total_size / (1024 * 1024) / elapsed_time);
            exit(0);
    }
}