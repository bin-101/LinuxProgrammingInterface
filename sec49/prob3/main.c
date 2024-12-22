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
2200バイトのファイルを作成
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob3$ dd if=/dev/zero bs=2200 count=1 | tr '\0' 'a' >file2200
1+0 レコード入力
1+0 レコード出力
2200 bytes (2.2 kB, 2.1 KiB) copied, 3.8821e-05 s, 56.7 MB/s
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob3$ ll file2200
-rw-rw-r-- 1 bin101 bin101 2200 12月 22 19:25 file2200

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob3$ gcc main.c
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob3$ ./a.out
page size: 4096
value at 2180: a
value at 4000: 
signal(7): Bus error
signal(7): Bus error
signal(7): Bus error
signal(7): Bus error # 何回も出力された。

8000をコメントアウトし、handlerにexit(1)を追加して実行
241664でセグフォが起きた。おそらく、8192~241663は偶然読み取り可能な領域だった。
~~~~~
value at 241658: 
value at 241659: 
value at 241660: 
value at 241661: 
value at 241662: 
value at 241663: 
signal(11): Segmentation fault

読み取りしか許可していない共有マッピングに書き込みをやってみる。以下を追加
```
    char *c=addr+10;
    *c='a';
```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec49/prob3$ ./a.out
page size: 4096
value at 2180: a
value at 4000: 
signal(11): Segmentation fault
*/


void handler(int sig){
    printf("signal(%d): %s\n",sig,strsignal(sig));
    exit(1);
}

void access_addr(void *addr,int idx){
    char *c=addr+idx;
    printf("value at %d: %c\n",idx,*c);
}

int main(){
    char file[]="./file2200";
    int fd=open(file,O_RDONLY);
    void *addr=mmap(NULL,8192,PROT_READ,MAP_SHARED,fd,0);

    struct sigaction sa;
    sa.sa_handler=handler;
    sigfillset(&sa.sa_mask);
    sa.sa_flags=0;
    if(sigaction(SIGBUS,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }
    if(sigaction(SIGSEGV,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }
    int page_size=sysconf(_SC_PAGE_SIZE);
    printf("page size: %d\n",page_size);
    access_addr(addr,2180);
    access_addr(addr,4000);
    //access_addr(addr,8000);
    // for(int i=8193;;i++){
    //     access_addr(addr,i);
    // }
    char *c=addr+10;
    *c='a';
}