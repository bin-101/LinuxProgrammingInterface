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
#include <ftw.h>
#include <time.h>

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob2$ ./a.out
st.st_atime: Wed Nov  6 20:36:33 2024
st.st_mtime: Wed Nov  6 20:36:33 2024
st.st_ctime: Wed Nov  6 20:36:33 2024
st.st_atime: Wed Nov  6 20:36:33 2024
st.st_mtime: Wed Nov  6 20:36:33 2024
st.st_ctime: Wed Nov  6 20:36:33 2024

変更されないことを確認した。
アクセス時刻は、i-nodeテーブル参照では更新されず、データブロックへのアクセス時に更新される？
*/

int main(){
    struct stat st;
    char path[]="file.txt";
    int res=stat(path,&st);
    if(res==-1){
        printf("%s\n",strerror(errno));
        return 1;
    }
    printf("st.st_atime: %s",ctime(&st.st_atime)); //最終アクセス時刻
    printf("st.st_mtime: %s",ctime(&st.st_mtime)); //最終変更時刻
    printf("st.st_ctime: %s",ctime(&st.st_ctime)); //属性最終変更時刻


    sleep(10);

    printf("st.st_atime: %s",ctime(&st.st_atime)); //最終アクセス時刻
    printf("st.st_mtime: %s",ctime(&st.st_mtime)); //最終変更時刻
    printf("st.st_ctime: %s",ctime(&st.st_ctime)); //属性最終変更時刻
}