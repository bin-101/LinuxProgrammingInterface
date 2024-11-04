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
#include <libgen.h>

const int NUM_SECTIONS=64;

int is_solved[65][20];
int solved_problems=0;


int calc_is_solved(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf){
    char path[PATH_MAX];
    strcpy(path,pathname);
    char *base0=basename(strdup(path));
    if(strcmp(base0,"AC")==0){
        char *base1=basename(dirname(strdup(path)));
        char *base2=basename(dirname(dirname(strdup(path))));
        int sec,prob;
        sscanf(base2,"sec%d",&sec);
        sscanf(base1,"prob%d",&prob);
        is_solved[sec][prob]=1;
        solved_problems++;
    }else if(strcmp(base0,"tlpi-240311-dist")==0){
        //ここは探索しない
        return FTW_SKIP_SUBTREE;
    }
    return 0;
}


int main(){
    int num_problems[NUM_SECTIONS+1];
    int fd=open("num_problems.txt", O_RDONLY, 0777);
    char buf[64*5];
    int n=read(fd, buf, 64*5);
    if(n==-1){
        printf("%s\n",strerror(errno));
        exit(1);
    }

    char *num_str=strtok(buf, "\n"); // strtok: https://www.gavo.t.u-tokyo.ac.jp/~dsk_saito/lecture/software2/misc/misc03.html
    for(int i=1;i<=NUM_SECTIONS;i++){
        num_problems[i]=atoi(num_str);
        if(i<NUM_SECTIONS-1) num_str=strtok(NULL, "\n");
    }

    if(nftw(".",calc_is_solved,20,FTW_PHYS|FTW_ACTIONRETVAL)==-1){
        printf("%s\n",strerror(errno));
        exit(1);
    }

    int fd_readme=open("README.md", O_WRONLY | O_CREAT | O_EXCL, 0777);
    char title[]="# The Linux Programming Interface Problems\n";
    write(fd_readme, title, strlen(title));

    int sum_problems=0;
    for(int i=1;i<=NUM_SECTIONS;i++){
        sum_problems+=num_problems[i];
    }
    char status[10000]= "## Status\n";
    snprintf(status, sizeof(status), "## Status\nsolved: %d\ntotal : %d\n", solved_problems, sum_problems);
    write(fd_readme, status, strlen(status));

    char list[100000]="## List\n";
    for(int i=1;i<=NUM_SECTIONS;i++){
        char list_sec[10000];
        int solved=0;
        for(int j=1;j<=num_problems[i];j++){
            if(is_solved[i][j]) solved++;
        }
        snprintf(list_sec, sizeof(list_sec), "- section %d solved: %d all: %d\n", i, solved, num_problems[i]);
        if(num_problems[i]){
            strcat(list_sec, "\t-");
        }
        for(int j=1;j<=num_problems[i];j++){
            char temp[1000];
            if(is_solved[i][j]){
                snprintf(temp, sizeof(temp), " :white_check_mark: <span style=\"color: green\">%d-%d</span> ", i, j);
            }else{
                snprintf(temp, sizeof(temp), " :white_medium_square: %d-%d ", i, j);
            }
            strcat(list_sec, temp);
        }
        strcat(list_sec, "\n");
        strcat(list, list_sec);
    }
    write(fd_readme, list, strlen(list));
}