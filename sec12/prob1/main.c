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

// p166の丸写し　全然わかっていない
// エラーの場合,-1を返す
uid_t userIdFromName(const char *name){
    if(name==NULL || *name=='\0'){
        return -1;
    }

    char *endptr;
    uid_t u=strtol(name,&endptr,10);
    if(*endptr=='\0'){
        return u;
    }

    struct passwd *pwd=getpwnam(name);
    if(pwd==NULL){
        return -1;
    }
    return pwd->pw_uid;
}

int main(int argc, char *argv[])
{
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
        if(entry->d_type!=DT_DIR){
            continue;
        }
        char *endptr;
        long pid = strtol(entry->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }
        printf("pid: %ld\n", pid);
        char status_path[256];
        // sizeof(status_path): 最大サイズ
        snprintf(status_path, sizeof(status_path), "/proc/%ld/status", pid);

        int fd = open(status_path, O_RDONLY);
        if (fd == -1) {
            perror("open");
            continue;
        }
        char line[100000];
        ssize_t nread;
        while ((nread = read(fd, line, sizeof(line))) > 0) {
            //printf("line: %s\n", line);
            char *new_line=strtok(line, "\n");
            while(new_line!=NULL){
                //printf("new_line: %s\n\n", new_line);
                if(strncmp(new_line, "Uid", 3)==0){
                    char *proc_uid = strtok(new_line, "\t");
                    if(proc_uid==NULL){
                        break;
                    }
                    proc_uid = strtok(NULL, "\t");
                    while(proc_uid!=NULL){
                        //printf("proc_uid: %s\n", proc_uid);
                        if(atoi(proc_uid)==uid){
                            printf("Process %ld belongs to UID %d\n", pid, uid);
                            break;
                        }
                        proc_uid = strtok(NULL, "\t");
                    }
                }
                new_line=strtok(NULL, "\n");
            }

            // if (strncmp(line, "Uid", 3) != 0) {
            //     continue;
            // }
            // printf("line: %s\n", line);
            // char *token = strtok(line, "\t");
            // printf("token: %s\n", token);
            // token = strtok(NULL, "\t");
            // uid_t proc_uid;
            // sscanf(token, "%d", &proc_uid);
            // if (proc_uid == uid) {
            //     printf("Process %ld belongs to UID %d\n", pid, uid);
            // }
            // break;
        }
        close(fd);
    }

    return 0;
}
/*
struct dirent {
    ino_t          d_ino;       // inode number
    char           d_name[256]; // filename
    unsigned char?  d_type;      // type of file
        DT_DIR ディレクトリ
        DT_REG 通常のファイル
};
struct dirent *readdir(DIR *dirp);
エラー時もしくは終端に到達した場合、NULLを返す

ps -uで出力があっているか確認？
*/