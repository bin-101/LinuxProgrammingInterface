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


/*
nftwを使用して、ファイルの種類別の数をカウントする。

nftw(const char *dirpath,
    int (*func) (const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf),
    int nopenfd, int flags)

- func
    - pathname: ファイルのパス名
    - statbuf: ファイルの情報 15.1章参照
    - typeflag: ファイルの種類
        - これをカウントすれば良さそう

出力
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec18/prob7$ ./a.out /home/bin101/code/LinuxProgrammingInterface/sec18
FTW_D  : 8
FTW_DNR: 0
FTW_DP : 0
FTW_F  : 14
FTW_NS : 0
FTW_SL : 3
FTW_SLN: 0
*/

int count[7];

int count_typeflag(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf){
    int type=0;
    switch(typeflag){
        case FTW_D: // ディレクトリ
            count[0]++;
            break;
        case FTW_DNR: // 読み込み不可能なディレクトリ
            count[1]++;
            break;
        case FTW_DP: // ディレクトリ。配下はすべて処理済み
            count[2]++;
            break;
        case FTW_F: // 通常のファイル
            count[3]++;
            break;
        case FTW_NS: // stat関数が失敗
            count[4]++;
            break;
        case FTW_SL: // シンボリックリンク
            count[5]++;
            break;
        case FTW_SLN: // dangling link
            count[6]++;
            break;
    }
    return 0; // 戻り値が0ならnftwは処理を継続
}

int main(int argc, char *argv[]){
    char path[PATH_MAX];
    if(argc<2){
        perror("argc");
    }
    if(nftw(argv[1],count_typeflag,10,FTW_PHYS)==-1){
        printf("%s\n",strerror(errno));
        exit(1);
    }
    printf("FTW_D  : %d\n",count[0]);
    printf("FTW_DNR: %d\n",count[1]);
    printf("FTW_DP : %d\n",count[2]);
    printf("FTW_F  : %d\n",count[3]);
    printf("FTW_NS : %d\n",count[4]);
    printf("FTW_SL : %d\n",count[5]);
    printf("FTW_SLN: %d\n",count[6]);

}