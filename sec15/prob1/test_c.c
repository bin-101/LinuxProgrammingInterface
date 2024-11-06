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


int make_dir(){
    for(int i=0;i<8;i++){
        char filename[100];
        sprintf(filename, "dir%d", i);
        if(mkdir(filename, 077 + i*64) == -1){
            return -1;
        }
    }
    return 0;
}

int make_file(){
    for(int i=0;i<8;i++){
        char filename[100];
        sprintf(filename, "file%d", i);
        int fd=open(filename, O_CREAT|O_EXCL, 077 + i*64);
        if(fd==-1){
            return -1;
        }
        close(fd);
    }
    return 0;
}

int calc_perm(int perm){
    return 077 + perm*64;
}

// 使いまわす
char dirname[100];
char filename[100];

void dir_file_name(int dir_perm, int file_perm, char *dirname, char *filename){
    sprintf(dirname, "dir%d", dir_perm);
    sprintf(filename, "%s/file%d", dirname, file_perm);
}

int make_all_dir_file(){
    for(int dir_perm=0;dir_perm<8;dir_perm++){
        dir_file_name(dir_perm, 0, dirname, filename);
        mkdir(dirname, 0777);
        for(int file_perm=0;file_perm<8;file_perm++){
            dir_file_name(dir_perm, file_perm, dirname, filename);
            int fd=open(filename, O_CREAT|O_EXCL, calc_perm(file_perm)); 
            if(fd==-1){
                printf("open error\n");
                exit(1);
            }
            close(fd);
        }
        chmod(dirname, calc_perm(dir_perm));
    }
}

int test_make_file(int dir_perm, int file_perm){
    dir_file_name(dir_perm, file_perm, dirname, filename);
    int fd=open(filename, O_CREAT|O_EXCL, calc_perm(file_perm)); 
    if(fd==-1){
        return -1;
    }
    close(fd);
    return 0;
}

int test_open_read_file(int dir_perm,int file_perm){
    dir_file_name(dir_perm, file_perm, dirname, filename);
    int fd=open(filename, O_RDONLY);
    if(fd==-1){
        return -1;
    }
    close(fd);
    return 0;
}

int test_open_write_file(int dir_perm,int file_perm){
    dir_file_name(dir_perm, file_perm, dirname, filename);
    int fd=open(filename, O_WRONLY);
    if(fd==-1){
        return -1;
    }
    close(fd);
    return 0;
}

int test_delete_file(int dir_perm, int file_perm){
    if(dir_perm==7 && file_perm==7){
        return 0;
    }
    dir_file_name(dir_perm, file_perm, dirname, filename);
    if(unlink(filename)==-1){
        return -1;
    }
    return 0;
}

int test_send_from(int dir_perm){
    dir_file_name(dir_perm,7,dirname,filename);
    char new_filename[]= "to_dir/file7";
    int res=rename(filename, new_filename);
    if(res==-1){
        return -1;
    }
    // ファイルを削除しておく
    if(unlink(new_filename)==-1){
        printf("%s\n",strerror(errno));
        return -1;
    }
    return 0;
}

int test_send_to(int perm,int is_dir_mode){
    if(is_dir_mode){
        dir_file_name(perm,7,dirname,filename);
        strcat(filename,"7");
    }else{
        dir_file_name(7,perm,dirname,filename);
    }
    char old_filename[]= "from_dir/file7";
    int res=rename(old_filename, filename);
    if(res==-1){
        return -1;
    }
    // ファイルを作っておく
    int fd=open(old_filename, O_CREAT|O_EXCL, 0777);
    close(fd);
    return 0;
}

int main(){
    umask(0);
    printf("test_make_file\n");
    printf("directory permission\n");
    for(int dir_perm=0;dir_perm<8;dir_perm++){
        mkdir("make_file_dir",0777);
        chdir("make_file_dir");
        make_dir();
        int res=test_make_file(dir_perm, 0);
        printf("%d: %d\n",dir_perm, res);
        chdir("..");
    }

    printf("file permission\n");
    for(int file_perm=0;file_perm<8;file_perm++){
        mkdir("make_file_file",0777);
        chdir("make_file_file");
        mkdir("dir7",0777);
        int res=test_make_file(7, file_perm);
        printf("%d: %d\n",file_perm, res);
        chdir("..");
    }
    
    // オープンのテスト準備
    mkdir("open_file",0777);
    chdir("open_file");
    make_all_dir_file();

    printf("test_open_read_file\n");
    printf("directory permission\n");
    for(int dir_perm=0;dir_perm<8;dir_perm++){
        int res=test_open_read_file(dir_perm, 7);
        printf("%d: %d\n",dir_perm, res);
    }

    printf("file permission\n");
    for(int file_perm=0;file_perm<8;file_perm++){
        int res=test_open_read_file(7, file_perm);
        printf("%d: %d\n",file_perm, res);
    }

    printf("test_open_write_file\n");
    printf("directory permission\n");
    for(int dir_perm=0;dir_perm<8;dir_perm++){
        int res=test_open_write_file(dir_perm, 7);
        printf("%d: %d\n",dir_perm, res);
    }
    
    printf("file permission\n");
    for(int file_perm=0;file_perm<8;file_perm++){
        int res=test_open_write_file(7, file_perm);
        printf("%d: %d\n",file_perm, res);
    }
    chdir("..");

    // ファイルの削除
    mkdir("delete_file",0777);
    chdir("delete_file");
    make_all_dir_file();
    printf("test_delete_file\n");
    printf("directory permission\n");

    for(int dir_perm=0;dir_perm<8;dir_perm++){
        int res=test_delete_file(dir_perm, 7);
        printf("%d: %d\n",dir_perm, res);
    }

    printf("file permission\n");
    for(int file_perm=0;file_perm<8;file_perm++){
        int res=test_delete_file(7, file_perm);
        printf("%d: %d\n",file_perm, res);
    }
    chdir("..");

    // 移動元ディレクトリ
    mkdir("send_from",0777);
    chdir("send_from");
    make_all_dir_file();
    mkdir("to_dir",0777);
    printf("test_send_from\n");
    for(int dir_perm=0;dir_perm<8;dir_perm++){
        int res=test_send_from(dir_perm);
        printf("%d: %d\n",dir_perm, res);
    }
    chdir("..");

    // 移動先ディレクトリ,ファイル
    mkdir("send_to",0777);
    chdir("send_to");
    make_all_dir_file();
    mkdir("from_dir",0777);
    int fd7=open("from_dir/file7", O_CREAT|O_EXCL, 0777);
    close(fd7);
    printf("test_send_to\n");

    printf("directory permission\n");
    for(int dir_perm=0;dir_perm<8;dir_perm++){
        int res=test_send_to(dir_perm,1);
        printf("%d: %d\n",dir_perm, res);
    }

    printf("file permission\n");
    for(int file_perm=0;file_perm<8;file_perm++){
        int res=test_send_to(file_perm,0);
        printf("%d: %d\n",file_perm, res);
    }
    chdir("..");

}