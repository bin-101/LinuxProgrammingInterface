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
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob6$ ls -l
合計 20
-rwxrwxr-x 1 bin101 bin101 16264 11月  6 23:05 a.out
-rw-rw-r-- 1 bin101 bin101   855 11月  6 23:05 main.c
---------- 1 bin101 bin101     0 11月  6 23:03 test.txt
-r-x------ 1 bin101 bin101     0 11月  6 23:05 test2.txt
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob6$ ./a.out test.txt
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob6$ ./a.out test2.txt
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob6$ ls -l
合計 20
-rwxrwxr-x 1 bin101 bin101 16264 11月  6 23:05 a.out
-rw-rw-r-- 1 bin101 bin101   855 11月  6 23:05 main.c
-r--r--r-- 1 bin101 bin101     0 11月  6 23:03 test.txt
-r-xr-xr-x 1 bin101 bin101     0 11月  6 23:05 test2.txt
*/

int chmod_arX(char *pathname){
    struct stat st;
    int res=stat(pathname,&st);
    if(res==-1){
        return -1;
    }
    mode_t new_perm=st.st_mode;
    new_perm=new_perm|S_IRUSR|S_IRGRP|S_IROTH;
    if((st.st_mode&S_IXUSR) || (st.st_mode&S_IXGRP) || (st.st_mode&S_IXOTH)){
        new_perm=new_perm|S_IXUSR|S_IXGRP|S_IXOTH;
    }
    res=chmod(pathname,new_perm);
    return res;
}

int main(int argc, char *argv[]){
    if(argc!=2){
        perror("argc");
        return 1;
    }
    if(chmod_arX(argv[1])){
        printf("%s\n",strerror(errno));
        return 1;
    }
}