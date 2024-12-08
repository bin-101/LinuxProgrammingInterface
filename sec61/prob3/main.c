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
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <assert.h>
#include <unistd.h>


#define BUF_SIZE 10000000LL
#define loop 1000LL

/*
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);

実行結果
naive(readとwrite)が他2つより遅いことを確認できた
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob3$ time ./a.out sendfile
offset: 10000000000

real    0m10.506s
user    0m0.001s
sys     0m8.089s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob3$ gcc main.c
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob3$ time ./a.out fast
offset: 10000000000

real    0m10.108s
user    0m0.000s
sys     0m7.896s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob3$ time ./a.out naive
offset: 10000000000

real    0m11.462s
user    0m0.001s
sys     0m8.995s

出力結果が正しいことを確認した。diffはメモリ不足のためか強制終了してしまった
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob3$ diff output_fast.txt output_naive.txt 
強制終了
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob3$ md5sum output_fast.txt 
21c28fa3d8bde9cc9cd01f5d170f4fe9  output_fast.txt
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob3$ md5sum output_naive.txt 
21c28fa3d8bde9cc9cd01f5d170f4fe9  output_naive.txt
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob3$ md5sum output_sendfile.txt 
21c28fa3d8bde9cc9cd01f5d170f4fe9  output_sendfile.txt
*/
char buf[BUF_SIZE];

ssize_t sendfile_fast(int out_fd, int in_fd, off_t *offset, size_t count){
    return copy_file_range(in_fd,offset,out_fd,NULL,count,0);
}

ssize_t sendfile_naive(int out_fd, int in_fd, off_t *offset, size_t count){
    // オフセット調整
    off_t save_ofs;
    if(offset!=NULL){
        save_ofs=lseek(in_fd,0,SEEK_CUR);
        if(save_ofs==-1){
            printf("lseek: %s\n",strerror(errno));
            return -1;
        }
        if(lseek(in_fd,*offset,SEEK_SET)==-1){
            return -1;
        }
    }
    ssize_t sum_num_write=0;
    while(count>0){
        ssize_t size=BUF_SIZE<count?BUF_SIZE:count;
        ssize_t num_read=read(in_fd,buf,size);
        if(num_read<=0){
            printf("read: %s\n",strerror(errno));
        }
        ssize_t num_write=write(out_fd,buf,num_read);
        if(num_write<=0){
            printf("write: %s\n",strerror(errno));
        }
        // assert(num_write==num_read);
        sum_num_write+=num_write;
        count-=num_read;
    }

    if(offset!=NULL){
        *offset=lseek(in_fd,0,SEEK_CUR);
        if(*offset==-1){
            return -1;
        }
        if(lseek(in_fd,save_ofs,SEEK_SET)==-1){
            return -1;
        }
    }
    return sum_num_write;
}

int main(int argc,char *argv[]){
    if(strcmp(argv[1],"make")==0){
        int fd=open("input.txt",O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
        for(int l=0;l<loop;l++){
            for(int i=0;i<BUF_SIZE;i++){
                buf[i]='a'+rand()%26;
            }
            write(fd,buf,BUF_SIZE);
        }
        close(fd);
        exit(0);
    }

    int fd=open("input.txt",O_RDONLY);
    int fd2;
    off_t offset=0;
    if(strcmp(argv[1],"fast")==0){
        fd2=open("output_fast.txt",O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
        while(offset<BUF_SIZE*loop){
            sendfile_fast(fd2,fd,&offset,BUF_SIZE*loop-offset);
        }
    }
    else if(strcmp(argv[1],"naive")==0){
        fd2=open("output_naive.txt",O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
        sendfile_naive(fd2,fd,&offset,BUF_SIZE*loop);
    }else{
        fd2=open("output_sendfile.txt",O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
        while(offset<BUF_SIZE*loop){
            sendfile(fd2,fd,&offset,BUF_SIZE*loop-offset);
        }
    }
    printf("offset: %ld\n",offset);
    close(fd);
    close(fd2);
    exit(0);
    

}