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

/*
最初にt_stat.cを写経

struct timespec{
    time_t tv_sec;
    long tv_nsec;
}

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob3$ ./a.out
File type:      regular file
Device containing i-node: major=259 minor=7
I-node number: 27399189
Mode: 100775
Number of (hard) links: 1
Ownership: UID=1000 GID=1000
File size: 16424 bytes
Optimal I/O block size: 16424 bytes
512B blocks allocated: 40
Last file access: Sat Jan 25 20:12:08 2025 0.420086775 seconds
Last file modification: Sat Jan 25 20:12:07 2025 0.707088851 seconds
Last status change: Sat Jan 25 20:12:07 2025 0.707088851 seconds
*/

static void
displayStatInfo(const struct stat *sb){
    printf("File type:      ");
    
    switch(sb->st_mode & S_IFMT){
        case S_IFREG: printf("regular file\n");         break;
        case S_IFDIR: printf("directory\n");            break;
        case S_IFCHR: printf("charcter device\n");      break;
        case S_IFBLK: printf("block device\n");         break;
        case S_IFLNK: printf("symbolic (soft) link\n"); break;
        case S_IFIFO: printf("FIFO or pipe\n");         break;
        case S_IFSOCK: printf("socket\n");              break;
        default: printf("unknown file type?\n");        break;
    }

    printf("Device containing i-node: major=%ld minor=%ld\n",
                    (long)major(sb->st_dev),(long)minor(sb->st_dev));
    printf("I-node number: %ld\n",(long)sb->st_ino);
    printf("Mode: %lo\n",(unsigned long)sb->st_mode);
    if(sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX)){
        printf("special bits set: %s%s%s\n",
                (sb->st_mode & S_ISUID)?"set-UID": "",
                (sb->st_mode & S_ISGID)?"set-GID":"",
                (sb->st_mode & S_ISVTX)?"sticky":"");
    }
    printf("Number of (hard) links: %ld\n",(long)sb->st_nlink);
    printf("Ownership: UID=%ld GID=%ld\n",
        (long)sb->st_uid,(long)sb->st_gid);
    
    printf("File size: %lld bytes\n",(long long)sb->st_size);
    printf("Optimal I/O block size: %lld bytes\n",(long long)sb->st_size);
    printf("512B blocks allocated: %lld\n",(long long)sb->st_blocks);

    char *str=ctime(&sb->st_atime); str[strlen(str)-1]='\0';
    printf("Last file access: %s 0.%09ld seconds\n",str,sb->st_atim.tv_nsec);

    str=ctime(&sb->st_mtime); str[strlen(str)-1]='\0';
    printf("Last file modification: %s 0.%09ld seconds\n",str,sb->st_mtim.tv_nsec);

    str=ctime(&sb->st_ctime); str[strlen(str)-1]='\0';
    printf("Last status change: %s 0.%09ld seconds\n",str,sb->st_ctim.tv_nsec);
}

int main(int argc,char *argv[]){
    struct stat sb;
    int statLink=(argc>1)&&strcmp(argv[1],"-l")==0;

    int fname=statLink?1:0;

    if(statLink){
        if(lstat(argv[fname],&sb)==-1){
            exit(1);
        }
    }else{
        if(stat(argv[fname],&sb)==-1){
            exit(1);
        }
    }
    displayStatInfo(&sb);
}