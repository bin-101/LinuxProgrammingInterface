#define _GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> // PATH_MAXが定義されている

/*
real_path(const char *pathname, char *resolved_path)
    pathname: パス名
    resolved_path: 実際のパス名を格納するためのバッファ
    pathnameがシンボリックリンクである場合、実際の絶対パス名をresolved_pathに格納する。
    シンボリックリンクでない場合、pathnameをresolved_pathに格納する。
これを実装する。


readlinkを使用する。
相対パスで返されるため、getcwdを使用して絶対パスに変換する。

出力
readlink    : dir
readlink_abs: /home/bin101/code/LinuxProgrammingInterface/sec18/prob3/dir
realpath    : /home/bin101/code/LinuxProgrammingInterface/sec18/prob3/dir
readlink    : dir/file
readlink_abs: /home/bin101/code/LinuxProgrammingInterface/sec18/prob3/dir/file
realpath    : /home/bin101/code/LinuxProgrammingInterface/sec18/prob3/dir/file
*/

void print_readlink(const char *pathname){
    char resolved_path[PATH_MAX];
    ssize_t len=readlink(pathname,resolved_path,PATH_MAX-1);
    if(len==-1){
        printf("%s\n",strerror(errno));
        return;
    }
    resolved_path[len]='\0';
    printf("readlink    : %s\n",resolved_path);
}

void print_readlink_absolute(const char *pathname){
    char resolved_path[PATH_MAX];
    ssize_t len=readlink(pathname,resolved_path,PATH_MAX-1);
    if(len==-1){
        printf("%s\n",strerror(errno));
        return;
    }
    resolved_path[len]='\0';

    char *cwd=getcwd(NULL,0);
    if(cwd==NULL){
        printf("%s\n",strerror(errno));
        return;
    }
    printf("readlink_abs: %s/%s\n",cwd,resolved_path);
    free(cwd);
}

void print_real_path(const char *pathname){
    char resolved_path[PATH_MAX];
    char *res=realpath(pathname,resolved_path);
    if(res==NULL){
        printf("%s\n",strerror(errno));
        return;
    }
    printf("realpath    : %s\n",resolved_path);
}

int main(){
    mkdir("dir",S_IRUSR|S_IWUSR|S_IXUSR);
    int fd=open("./dir/file",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
    symlink("dir","./dir_link");
    symlink("dir/file","./file_link");

    print_readlink("./dir_link");
    print_readlink_absolute("./dir_link");
    print_real_path("./dir_link");
    print_readlink("./file_link");
    print_readlink_absolute("./file_link");
    print_real_path("./file_link");
}

