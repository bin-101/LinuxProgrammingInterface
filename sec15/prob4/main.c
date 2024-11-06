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
ディレクトリの実行権限があると仮定

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob4$ ./a.out
-1
-1
0
*/
int my_access(const char *pathname, int mode){
    // if(mode==0) return 0; // F_OKが0として定義されている
    struct stat st;
    int res=stat(pathname,&st);
    if(res==-1){
        return -1;
    }
    int perm=0;
    if(geteuid()==st.st_uid){
        perm=(st.st_mode>>6)&7;
    }else if(getegid()==st.st_gid){
        perm=(st.st_mode>>3)&7;
    }else{
        perm=(st.st_mode)&7;
    }
    if((mode&R_OK) && (perm&4)==0) return -1;
    if((mode&W_OK) && (perm&2)==0) return -1;
    if((mode&X_OK) && (perm&1)==0) return -1;
    return 0;
}

int main(){
    int fd=open("file0477", O_CREAT|O_EXCL, 0477);
    close(fd);
    printf("%d\n",my_access("file0477",R_OK|X_OK));
    printf("%d\n",my_access("file0577",F_OK)); // 0として定義されている
    printf("%d\n",my_access("file0477",R_OK));
    return 0;
}