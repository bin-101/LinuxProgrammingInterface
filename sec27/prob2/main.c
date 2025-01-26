#define _GNU_SOURCE
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

/*
参考
-可変長引数
    https://qiita.com/subaruf/items/657c67a1809515589a7c
- execlp.c

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec27/prob2$ ./a.out
ap0: ls
ap0: -l
ap0: -a
pathname: /usr/local/sbin/ls
args: ls -l -a
pathname: /usr/local/bin/ls
args: ls -l -a
pathname: /usr/sbin/ls
args: ls -l -a
pathname: /usr/bin/ls
args: ls -l -a
total 32
drwxrwxr-x 2 bin101 bin101  4096 Jan 26 13:27 .
drwxrwxr-x 7 bin101 bin101  4096 Jan 26 00:13 ..
-rwxrwxr-x 1 bin101 bin101 16472 Jan 26 13:27 a.out
-rw-rw-r-- 1 bin101 bin101  2246 Jan 26 13:27 main.c
*/

#define FREE free(args);free(envs)

int _execlp(const char *filename,const char *arg,...){
    va_list ap;
    va_start(ap,arg);

    int num_arg=0;
    const char *now=arg;
    while(now!=NULL){
        printf("ap0: %s\n",now);
        num_arg++;
        now=va_arg(ap,char *);
    }

    int num_env=0;
    now=va_arg(ap,char *);
    while(now!=NULL){
        printf("ap1: %s\n",(char *)ap);
        num_env++;
        now=va_arg(ap,char *);
    }
    va_end(ap);

    char **args=malloc(sizeof(char *)*(num_arg+1));
    char **envs=malloc(sizeof(char *)*(num_env+1));
    va_start(ap,arg);

    int id=0;
    now=arg;
    while(now!=NULL){
        args[id]=now;
        id++;
        now=va_arg(ap,char *);
    }
    args[id]=NULL;

    id=0;
    now=va_arg(ap,char *);
    while(now!=NULL){
        envs[id]=now;
        id++;
        now=va_arg(ap,char *);
    }
    envs[id]=NULL;
    va_end(ap);

    if(strchr(filename,'/')!=NULL){
        int res=execve(filename,args,envs);
        FREE;
        return res;
    }

    char *p=getenv("PATH");
    char *path;
    if(p==NULL){
        path=strdup(".:/usr/bin:/bin");
    }else{
        path=strdup(p);
    }

    char *pStart=path;
    while(1){
        char *pEnd=strchr(pStart,':');
        int is_end=0;
        if(pEnd==NULL){
            is_end=1;
            pEnd=pStart+strlen(pStart);
        }
        char *pathname;
        pathname=malloc(pEnd-pStart+strlen(filename)+2);
        pathname[0]='\0';
        strncat(pathname,pStart,pEnd-pStart);
        strcat(pathname,"/");
        strcat(pathname,filename);

        printf("pathname: %s\n",pathname);
        printf("args: %s %s %s\n",args[0],args[1],args[2]);

        execve(pathname,args,envs);
        free(pathname);
        if(is_end) break;
        pStart=pEnd+1;
    }
    free(path);
    FREE;
    return -1;
}

int main() {
    _execlp("ls", "ls", "-l", "-a", NULL,NULL);
    perror("_execlp");
}
