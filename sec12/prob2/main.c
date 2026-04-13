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

typedef struct{
    pid_t pid;
    pid_t ppid;
    char name[1000];
    int printed;
} proc_info;

proc_info proc_infos[1000000];
int proc_info_count=0;

void dfs(pid_t ppid,int depth) {
    for(int i=0; i<proc_info_count; i++){
        if(proc_infos[i].ppid==ppid && proc_infos[i].printed==0){
            for(int j=0; j<depth; j++){
                printf("  ");
            }
            printf("%d %s\n", proc_infos[i].pid, proc_infos[i].name);
            proc_infos[i].printed=1;
            dfs(proc_infos[i].pid, depth+1);
        }
    }
}

int main(){
    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL) {
        char *endptr;
        pid_t pid = strtol(entry->d_name, &endptr, 10);
        if (*endptr != '\0') { // 数字以外が含まれている場合
            continue;
        }
        char status_path[PATH_MAX];
        snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);
        int fd = open(status_path, O_RDONLY);
        if (fd == -1) {
            perror("open");
            continue; // ファイルが途中で消えることがあるため、エラーが起きても続行
        }
        const int buf_size = 1000000;
        char buf[buf_size];
        int nread=read(fd, buf, sizeof(buf)-1); // '\0'を入れるために-1
        
        close(fd);
        if(nread==-1 || nread==buf_size-1){
            perror("read");
            continue; // ファイルが途中で消えることがあるため、エラーが起きても続行
        }
        buf[nread]='\0'; // strtok()は'\0'まで探索する
        char *line=strtok(buf, "\n");
        while(line!=NULL){
            if(strncmp(line, "PPid:", 5)==0){
                pid_t ppid= (pid_t)strtol(line+5, &endptr, 10); // strtol は最初のスペースを無視して数字を読み取るため、line+5でスペースを飛ばす必要はない
                if(line+5==endptr){ // 数字がない場合
                    fprintf(stderr, "invalid PPid line: %s\n", line);
                    exit(EXIT_FAILURE);
                }
                proc_infos[proc_info_count].ppid=ppid;
            }
            if(strncmp(line, "Pid:", 4)==0){
                pid_t pid= (pid_t)strtol(line+4, &endptr, 10);
                if(line+4==endptr){ // 数字がない場合
                    fprintf(stderr, "invalid Pid line: %s\n", line);
                    exit(EXIT_FAILURE);
                }
                proc_infos[proc_info_count].pid=pid;
            }
            if(strncmp(line, "Name:", 5)==0){
                for(char *p=line+5; *p!='\0'; p++){
                    if(!isspace(*p)){
                        strncpy(proc_infos[proc_info_count].name, p,sizeof(proc_infos[proc_info_count].name) - 1);
                        break;
                    }
                }
            }
            line = strtok(NULL, "\n");
        }
        proc_info_count++;

    }
    dfs(0, 0); // initプロセスのPPidは0
    printf("missing parent processes:\n");
    for(int i=0; i<proc_info_count; i++){
        if(proc_infos[i].printed==0){
            printf("pid:%d ppid:%d %s\n", proc_infos[i].pid, proc_infos[i].ppid, proc_infos[i].name);
        }
    }
}
