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
#include <sys/file.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob4$ ls -i fcntl.txt 
27155230 fcntl.txt # i-node番号を確認

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob4$ ./55-1 ./fcntl.txt e 100 &
[4] 146138
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob4$ PID 146138: requesting LOCK_EX at Tue Dec 31 21:27:26 2024
PID 146138: granted LOCK_EX at Tue Dec 31 21:27:26 2024 # flockでロック成功

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob4$ ./55-2 ./fcntl.txt
Enter ? for help
PID=146237> s w 0 0
[PID=146237] got lock # fcntlでロック成功

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec55/prob4$ cat /proc/locks | grep 27155230
37: FLOCK  ADVISORY  WRITE 146138 103:07:27155230 0 EOF
64: POSIX  ADVISORY  WRITE 146237 103:07:27155230 0 EOF # 両方表示されることを確認。

flockとfcntlは互いに影響しないことが分かった
*/


#define MAX_LINE 100

int main(int argc,char *argv[]){
    int fd=open(argv[1],O_RDWR);
    if(fd==-1){
        perror("open");
        exit(1);
    }

    printf("Enter ? for help\n");

    for(;;){
        printf("PID=%ld> ",(long)getpid());
        fflush(stdout);

        char line[LINE_MAX];
        if(fgets(line,MAX_LINE,stdin)==NULL){
            exit(0);
        }
        line[strlen(line)-1]='\0';

        if(*line=='\0') continue;

        char whence='s',cmdCh,lock;
        long long len,st;
        int numRead=sscanf(line,"%c %c %lld %lld %c",&cmdCh,&lock,&st,&len,&whence);
        struct flock fl;
        fl.l_start=st;
        fl.l_len=len;

        int cmd=(cmdCh=='g')?F_GETLK:(cmdCh=='s')?F_SETLK:F_SETLKW;
        fl.l_type=(lock=='r')?F_RDLCK:(lock=='w')?F_WRLCK:F_UNLCK;
        fl.l_whence=(whence=='c')?SEEK_CUR:(whence=='e')?SEEK_END:SEEK_SET;

        int status=fcntl(fd,cmd,&fl);

        if(cmd==F_GETLK){
            if(status==-1){
                perror("fcntl - F_GETLK");
                exit(1);
            }else{
                if(fl.l_type==F_UNLCK){
                    printf("[PID=%ld] Lock can be placed\n",(long)getpid());
                }else{
                    printf("[PID=%ld] Denied by %s lock on %lld:%lld "
                            "(held by PID %ld)\n",(long)getpid(),
                            (fl.l_type==F_RDLCK)?"READ":"WRITE",
                            (long long)fl.l_start,
                            (long long)fl.l_len,(long)fl.l_pid);
                }
            }
        }else{
            if(status==0){
                printf("[PID=%ld] %s\n",(long)getpid(),(lock=='u')?"unlocked":"got lock");
            }else if(errno==EAGAIN || errno==EACCES){
                printf("[PID=%ld] failed (incompatible lock)\n",(long)getpid());
            }else if(errno==EDEADLK){
                printf("[PID=%ld] failed (deadlock)\n",(long)getpid());
            }else{
                perror("fcntl - F_SETLK(W)");
                exit(1);
            }
        }
    }
}