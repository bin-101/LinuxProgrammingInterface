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
#include <limits.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/mman.h>

/*
実行結果
./a.out > output.txt
*/

int main(){
    int page_size=sysconf(_SC_PAGE_SIZE);
    char cs[4096*10]={};
    for(int i=0;i<3;i++){
        for(int j=0;j<page_size;j++){
            char cs2[2]={};
            cs2[0]='a'+i;
            strcat(cs,cs2);
        }
    }
    int fd=open("./input.txt",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
    if(fd==-1){
        perror("open");
        exit(1);
    }
    if(write(fd,cs,page_size*3)!=page_size*3){
        perror("write");
        exit(1);
    }

    void *addr_all=mmap(NULL,page_size*3,PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    if(addr_all==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    void *addr0=mmap(addr_all,page_size,PROT_READ,MAP_PRIVATE|MAP_FIXED,fd,page_size*2);
    if(addr0==MAP_FAILED){
        perror("addr0");
        exit(1);
    }
    void *addr1=mmap(addr_all+page_size,page_size,PROT_READ,MAP_PRIVATE|MAP_FIXED,fd,page_size);
    if(addr1==MAP_FAILED){
        perror("addr1");
        exit(1);
    }
    void *addr2=mmap(addr_all+page_size*2,page_size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_FIXED,fd,0);
    if(addr2==MAP_FAILED){
        perror("addr2");
        exit(1);
    }
    char *c=addr2+page_size-1;
    *c='\0';
    printf("%s\n",(char *)addr0);
}