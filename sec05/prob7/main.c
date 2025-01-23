#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
ssize_t readv(int fd,const struct iovec *iov,int iovcnt);

ssize_t writev(int fd,const struct iovec *iov,int iovcnt);
*/


ssize_t my_readv(int fd,const struct iovec *iov,int iovcnt){
    int size=0;
    for(int i=0;i<iovcnt;i++){
        size+=iov[i].iov_len;
    }
    void *buffer=malloc(size);
    ssize_t res=read(fd,buffer,size);
    int now_idx=0;
    for(int i=0;i<iovcnt;i++){
        memcpy(iov[i].iov_base,buffer+now_idx,iov[i].iov_len);
        now_idx+=iov[i].iov_len;
    }
    free(buffer);
    return res;    
}

// p108の方法。非効率らしいが、writevを使えない以上、こうするしかない？
ssize_t my_writev(int fd,const struct iovec *iov,int iovcnt){
    int size=0;
    for(int i=0;i<iovcnt;i++){
        size+=iov[i].iov_len;
    }
    void *buffer=malloc(size);
    int now_idx=0;
    for(int i=0;i<iovcnt;i++){
        memcpy(buffer+now_idx,iov[i].iov_base,iov[i].iov_len);
        now_idx+=iov[i].iov_len;
    }
    ssize_t res=write(fd,buffer,size);
    free(buffer);
    return res;
}

int main(){
    struct iovec iov[2];
    int x=10;
    char str[]={'h','e'};
    iov[0].iov_base=&x;
    iov[0].iov_len=sizeof(x);
    iov[1].iov_base=str;
    iov[1].iov_len=sizeof(char)*2;

    int fd=open("test.txt",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
    my_writev(fd,iov,2);

    lseek(fd,0,SEEK_SET);
    struct iovec iov2[2];
    iov2[0].iov_len=sizeof(int);
    iov2[1].iov_len=sizeof(char)*2;
    for(int i=0;i<2;i++){
        iov2[i].iov_base=malloc(iov2[i].iov_len);
    }
    my_readv(fd,iov2,2);
    printf("%d\n",*(int *)iov2[0].iov_base);
    printf("%s\n",(char *)iov2[1].iov_base);
}