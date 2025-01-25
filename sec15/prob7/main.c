#define _GNU_SOURCE
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/sysmacros.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob7$ sudo ./a.out +a chattr.txt
[sudo] bin101 のパスワード: 
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob7$ lsattr chattr.txt
-----a--------e------- chattr.txt
*/

int main(int argc,char *argv[]){
    int fd=open(argv[2],O_RDONLY);
    if(fd==-1){
        perror("open");
        exit(1);
    }

    int attr=0;
    for(int i=1;i<strlen(argv[1]);i++){
        char c=argv[1][i];
        switch(c){
            case 'a': attr|=FS_APPEND_FL;       break;
            case 'c': attr|=FS_COMPR_FL;        break;
            case 'D': attr|=FS_DIRSYNC_FL;      break;
            case 'i': attr|=FS_IMMUTABLE_FL;    break;
            case 'j': attr|=FS_JOURNAL_DATA_FL; break;
            case 'A': attr|=FS_NOATIME_FL;      break;
            case 'd': attr|=FS_NODUMP_FL;       break;
            case 't': attr|=FS_NOTAIL_FL;       break;
            case 's': attr|=FS_SECRM_FL;        break;
            case 'S': attr|=FS_SYNC_FL;         break;
            case 'T': attr|=FS_TOPDIR_FL;       break;
            case 'u': attr|=FS_UNRM_FL;         break;
            default: exit(1);
        }
    }

    int now_attr=0;
    if(ioctl(fd,FS_IOC_GETFLAGS,&now_attr)==-1){
        perror("ioctl get");
        exit(1);
    }
    if(argv[1][0]=='+'){
        now_attr|=attr;
    }else if(argv[1][0]=='-'){
        now_attr&=(~attr);
    }else if(argv[1][0]=='='){
        now_attr=attr;
    }
    if(ioctl(fd,FS_IOC_SETFLAGS,&now_attr)==-1){
        perror("ioctl set");
        exit(1);
    }
}