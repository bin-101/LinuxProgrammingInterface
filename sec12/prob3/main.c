#define _GNU_SOURCE
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec12/prob3$ sleep 100 > test.txt &
[1] 132982
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec12/prob3$ sleep 100 > test.txt &
[2] 132989
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec12/prob3$ ./a.out ./test.txt
pid: 132982
pid: 132989
*/

int is_same_file(char *a,char *b){
    char *real_a=realpath(a,NULL);
    char *real_b=realpath(b,NULL);
    if(real_a==NULL || real_b==NULL){
        // perror("realpath");
        return -1;
    }
    int res=strcmp(real_a,real_b);
    free(real_a);
    free(real_b);
    return res;
}

int main(int argc, char *argv[])
{
    if(argc!=2){
        perror("argc");
        exit(EXIT_FAILURE);
    }

    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL) {
        if(entry->d_type!=DT_DIR){ // Linuxの場合,フィールドd_typeが存在する
            continue;
        }
        char *endptr;
        long pid = strtol(entry->d_name, &endptr, 10);
        if (*endptr != '\0') { // 数字以外が含まれている場合
            continue;
        }

        char fd_path[256];
        snprintf(fd_path, sizeof(fd_path), "/proc/%ld/fd", pid);
        DIR *fd_dir = opendir(fd_path);
        if (fd_dir == NULL) {
            continue;
        }
        struct dirent *fd_entry;
        while ((fd_entry = readdir(fd_dir)) != NULL){
            if(fd_entry->d_type==DT_DIR){ // .と..を除外
                continue;
            }
            char fd_file[PATH_MAX]={};
            strncpy(fd_file, fd_path, sizeof(fd_path));
            strcat(fd_file, "/");
            strcat(fd_file, fd_entry->d_name);
            char real_file[PATH_MAX];
            ssize_t len=readlink(fd_file,real_file,PATH_MAX-1);
            real_file[len]='\0'; // readlinkは終端にヌル文字を付けない
            if(len==-1){
                printf("%s\n",strerror(errno));
                continue;
            }
            if(is_same_file(argv[1],real_file)==0){
                printf("pid: %ld\n",pid);
                break;
            }
        }
    }
    return 0;
}
