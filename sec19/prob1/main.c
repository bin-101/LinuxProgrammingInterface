#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/inotify.h>
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
vscode実行すると、最初に以下の出力が出た
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec19/prob1$ ./a.out test
Watching test/aaa wd: 1
Watching test/bbb wd: 2
Watching test wd: 3
    wd = 3; mask = IN_ISDIR IN_OPEN 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_ISDIR IN_OPEN 
        name = aaa
    wd = 1; mask = IN_ISDIR IN_OPEN 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = aaa
    wd = 1; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = aaa
    wd = 1; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_CLOSE_NOWRITE IN_ISDIR 
        name = aaa
    wd = 1; mask = IN_CLOSE_NOWRITE IN_ISDIR 
    wd = 3; mask = IN_ISDIR IN_OPEN 
        name = bbb
    wd = 2; mask = IN_ISDIR IN_OPEN 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = bbb
    wd = 2; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = bbb
    wd = 2; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_CLOSE_NOWRITE IN_ISDIR 
        name = bbb
    wd = 2; mask = IN_CLOSE_NOWRITE IN_ISDIR 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_CLOSE_NOWRITE IN_ISDIR 

しかし、端末で実行すると何も出なかった。vscodeの場合、左にディレクトリが表示されているのでそれが原因？
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec19/prob1$ ./a.out test
Watching test/aaa wd: 1
Watching test/bbb wd: 2
Watching test wd: 3

端末で実験する
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec19/prob1$ ./a.out test &
[1] 12370
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec19/prob1$ Watching test/aaa wd: 1
Watching test/bbb wd: 2
Watching test wd: 3

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec19/prob1$ rm -rf test/aaa
    wd = 3; mask = IN_ISDIR IN_OPEN 
        name = aaa
    wd = 1; mask = IN_ISDIR IN_OPEN 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = aaa
    wd = 1; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = aaa
    wd = 1; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_CLOSE_NOWRITE IN_ISDIR 
        name = aaa
    wd = 1; mask = IN_CLOSE_NOWRITE IN_ISDIR 
    wd = 1; mask = IN_DELETE_SELF 
    wd = 1; mask = IN_IGNORED 
Unwatching  wd: 1
    wd = 3; mask = IN_DELETE IN_ISDIR 
        name = aaa
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec19/prob1$ mkdir -p test/aaa/a2 # 2つとも監視対象になったことが確認できた
    wd = 3; mask = IN_CREATE IN_ISDIR 
        name = aaa
Watching test/aaa/a2 wd: 4
Watching test/aaa wd: 5
    wd = 3; mask = IN_ISDIR IN_OPEN 
        name = aaa
    wd = 3; mask = IN_CLOSE_NOWRITE IN_ISDIR 
        name = aaa
    wd = 3; mask = IN_ISDIR IN_OPEN 
        name = aaa
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = aaa
    wd = 3; mask = IN_CLOSE_NOWRITE IN_ISDIR 
        name = aaa
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec19/prob1$ rm -rf test/aaa
    wd = 3; mask = IN_ISDIR IN_OPEN 
        name = aaa
    wd = 5; mask = IN_ISDIR IN_OPEN 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = aaa
    wd = 5; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_CLOSE_NOWRITE IN_ISDIR 
        name = aaa
    wd = 5; mask = IN_CLOSE_NOWRITE IN_ISDIR 
    wd = 3; mask = IN_ISDIR IN_OPEN 
        name = aaa
    wd = 5; mask = IN_ISDIR IN_OPEN 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = aaa
    wd = 5; mask = IN_ACCESS IN_ISDIR 
    wd = 3; mask = IN_ACCESS IN_ISDIR 
        name = aaa
    wd = 5; mask = IN_ACCESS IN_ISDIR 
    wd = 5; mask = IN_ISDIR IN_OPEN 
        name = a2
    wd = 4; mask = IN_ISDIR IN_OPEN 
    wd = 5; mask = IN_ACCESS IN_ISDIR 
        name = a2
    wd = 4; mask = IN_ACCESS IN_ISDIR 
    wd = 5; mask = IN_ACCESS IN_ISDIR 
        name = a2
    wd = 4; mask = IN_ACCESS IN_ISDIR 
    wd = 5; mask = IN_CLOSE_NOWRITE IN_ISDIR 
        name = a2
    wd = 4; mask = IN_CLOSE_NOWRITE IN_ISDIR 
    wd = 4; mask = IN_DELETE_SELF 
    wd = 4; mask = IN_IGNORED 
Unwatching  wd: 4
    wd = 5; mask = IN_DELETE IN_ISDIR 
        name = a2
    wd = 3; mask = IN_CLOSE_NOWRITE IN_ISDIR 
        name = aaa
    wd = 5; mask = IN_CLOSE_NOWRITE IN_ISDIR 
    wd = 5; mask = IN_DELETE_SELF 
    wd = 5; mask = IN_IGNORED 
Unwatching  wd: 5
    wd = 3; mask = IN_DELETE IN_ISDIR 
        name = aaa
*/

const int MAX_WD=1000;
char pathname_watched[1000][PATH_MAX];
int inotify_fd;

int watch_dirs(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf){
    if(typeflag!=FTW_D && typeflag!=FTW_DP){
        printf("Not a directory\n");
        return 0;
    }
    
    for(int i=0;i<MAX_WD;i++){
        if(strcmp(pathname_watched[i],pathname)==0){
            printf("Already watched\n");
            // すでに挿入済み
            return 0;
        }
    }
    
    int wd=inotify_add_watch(inotify_fd,pathname,IN_ALL_EVENTS);
    if(wd==-1){
        printf("%s\n",strerror(errno));
        exit(1);
    }
    if(wd>=MAX_WD){
        printf("wd is too large\n");
        exit(1);
    }
    printf("Watching %s wd: %d\n",pathname,wd);
    strcpy(pathname_watched[wd],pathname);
    return 0;
}

// demo_inotify.cのコピー 便利なのでそのまま使う
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
static void             /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
    
    // 以下自分の実装
    if((i->mask & IN_CREATE) && (i->mask & IN_ISDIR)){ // ディレクトリが作成された
        char dir_name[PATH_MAX];
        strncpy(dir_name,pathname_watched[i->wd],PATH_MAX);
        strcat(dir_name,"/");
        strcat(dir_name,i->name);
        nftw(dir_name,watch_dirs,10,FTW_DEPTH);
    }

    // 削除された場合、自動的に監視対象から外されるためinotify_rm_watchは不要
    if(i->mask & IN_IGNORED){
        pathname_watched[i->wd][0]='\0';
        printf("Unwatching %s wd: %d\n",pathname_watched[i->wd],i->wd);
    }
}

int main(int argc, char *argv[]){
    char path[PATH_MAX];
    if(argc<2){
        perror("argc");
    }

    inotify_fd=inotify_init();
    if(inotify_fd==-1){
        printf("%s\n",strerror(errno));
        exit(1);
    }

    for(int i=1;i<argc;i++){
        if(nftw(argv[i],watch_dirs,10,FTW_DEPTH)==-1){
            printf("%s\n",strerror(errno));
            exit(1);
        }
    }

    while(1){
        char buf[BUF_LEN];
        ssize_t numRead=read(inotify_fd,buf,BUF_LEN);
        if(numRead<=0){
            printf("%s\n",strerror(errno));
            exit(1);
        }
        for(char *p=buf;p<buf+numRead;){
            struct inotify_event *event=(struct inotify_event *)p;
            displayInotifyEvent(event);
            p+=sizeof(struct inotify_event)+event->len;
        }
    }
}