#define _GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> // PATH_MAXが定義されている
#include <dirent.h>

/*
getcwd(char *cwdbuf, size_t size)
stat(const char *path, struct stat *statbuf) 15.1章

出力
/
/home/bin101/code/LinuxProgrammingInterface/sec18/prob5
/home/bin101/code/LinuxProgrammingInterface/sec18/prob5/
*/


void now_dir(char *path){
    struct stat st;
    int res=stat(".",&st);
    if(res==-1){
        printf("%s\n",strerror(errno));
        exit(1);
    }
    DIR *dir=opendir("..");
    while(1){
        struct dirent *dp=readdir(dir);
        if(dp==NULL) break;
        if(dp->d_ino==st.st_ino){ // カレントディレクトリのinode番号と一致する場合
            strcpy(path,dp->d_name);
            closedir(dir);
            return;
        }
    }
    closedir(dir);
    perror("now_dir");
}

// 1ならrootディレクトリ
// 0ならrootディレクトリではない
int is_root(){
    ino_t ino_cur=0;
    ino_t ino_par=1;

    DIR *dir=opendir(".");
    while(1){
        struct dirent *dp=readdir(dir);
        if(dp==NULL) break;
        if(strcmp(dp->d_name,".")==0){
            ino_cur=dp->d_ino;
        }else if(strcmp(dp->d_name,"..")==0){
            ino_par=dp->d_ino;
        }
    }
    closedir(dir);
    if(ino_cur==ino_par) return 1;
    return 0;
}

void my_getcwd(char *buf){
    char dir_name[PATH_MAX];
    while(is_root()==0){
        now_dir(dir_name);
        strcat(dir_name,"/");
        strcat(dir_name,buf);
        strcpy(buf,dir_name);
        chdir("..");
    }
    //bufの一番上の文字が/になるようにする
    memmove(buf+1,buf,strlen(buf)+1);
    buf[0]='/';

    system("pwd");
    int fd=open(buf,O_RDONLY);
    if(fd==-1){
        printf("%s\n",strerror(errno));
        exit(1);
    }
    fchdir(fd);
    system("pwd");
}

int main(){
    char buf[PATH_MAX]={};
    my_getcwd(buf);
    printf("%s\n",buf);
}