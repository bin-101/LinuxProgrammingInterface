#define _GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec18/prob2$ ls -al
合計 32
drwxrwxr-x 3 bin101 bin101  4096 11月  3 17:54 .
drwxrwxr-x 3 bin101 bin101  4096 11月  3 17:35 ..
-rwxrwxr-x 1 bin101 bin101 16176 11月  3 17:50 a.out
-rw-rw-r-- 1 bin101 bin101   500 11月  3 17:54 main.c
lrwxrwxrwx 1 bin101 bin101     6 11月  3 17:54 mylink -> myfile
drwx------ 2 bin101 bin101  4096 11月  3 17:54 test

mylinkはtest/myfileではなく、myfileを指しているため、dangling linkとなっている。
chmodはリンクをたどるため、No such file or directoryのエラーが発生する。
*/

int main(){
    mkdir("test",S_IRUSR|S_IWUSR|S_IXUSR);
    chdir("test"); // カレントディレクトリをtestディレクトリに変更
    int fd=open("myfile",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR); // myfileを作成
    symlink("myfile","../mylink");
    int res=chmod("../mylink",S_IRUSR);
    printf("%s\n",strerror(errno));
    printf("%d\n",res); // -1が出力される。つまりエラーが発生している。
}

