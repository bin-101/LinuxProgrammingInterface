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

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ sudo -iu outsider
[sudo] bin101 のパスワード: 
sudo: ディレクトリーを /home/outsider に変更できません: そのようなファイルやディレクトリはありません
$ sleep 100 &
$ sleep 100 &


bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec12/prob1$ ./a.out outsider
uid: 1001
pid: 128449, name: sh
pid: 128496, name: sleep
pid: 128501, name: sleep
*/

// ugid_functions.c からコピペ
uid_t           /* Return UID corresponding to 'name', or -1 on error */
userIdFromName(const char *name)
{
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    if (name == NULL || *name == '\0')  /* On NULL or empty string */
        return -1;                      /* return an error */

    u = strtol(name, &endptr, 10);      /* As a convenience to caller */
    if (*endptr == '\0')                /* allow a numeric string */
        return u;

    pwd = getpwnam(name);
    if (pwd == NULL)
        return -1;

    return pwd->pw_uid;
}

int main(int argc, char *argv[])
{
    if(argc!=2){
        perror("argc");
        exit(EXIT_FAILURE);
    }
    char* user_name=argv[1];
    uid_t uid=userIdFromName(user_name);
    printf("uid: %d\n", uid);


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
        char status_path[256];
        // sizeof(status_path): 最大サイズ
        snprintf(status_path, sizeof(status_path), "/proc/%ld/status", pid);

        int fd = open(status_path, O_RDONLY);
        if (fd == -1) {
            printf("open error: %s\n", status_path);
            continue;
        }
        const int buf_size = 1000000;
        char buf[buf_size];
        int nread=read(fd, buf, sizeof(buf));
        if(nread==-1 || nread==buf_size){
            perror("read");
            exit(EXIT_FAILURE);
        }
        
        char *line=strtok(buf, "\n");
        int ok_uid=0;
        char name[1000]={};
        while(line!=NULL){
            if(strncmp(line, "Uid", 3)==0){
                uid_t proc_uid= strtol(line+4, &endptr, 10);
                if(proc_uid==uid){
                    ok_uid=1;
                }
            }
            if(strncmp(line, "Name", 4)==0){
                for(char *p=line+5; *p!='\0'; p++){
                    if(!isspace(*p)){
                        strcat(name, p);
                        break;
                    }
                }
            }
            line=strtok(NULL, "\n");
            if(ok_uid==1){
                printf("pid: %ld, name: %s\n", pid, name);
                break;
            }
        }
        close(fd);
    }

    return 0;
}