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
#include <sys/time.h>
#include <pthread.h>
#include <sys/resource.h>
#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>

#define BD_NO_CHDIR 01
#define BD_NO_CLOSE_FILES 02
#define BD_NO_REOPEN_STD_FDS 04
#define BD_NO_UMASK0 010

#define BD_MAX_CLOSE 8192

/*
例37-2の丸写し
*/

int becomeDaemon(int flags){
    switch(fork()){
        case -1:
            return -1;
        case 0:
            break;
        default:
            _exit(EXIT_SUCCESS);
    }
    
    if(setsid() == -1){
        return -1;
    }

    switch(fork()){
        case -1:
            return -1;
        case 0:
            break;
        default:
            _exit(EXIT_SUCCESS);
    }

    if(!(flags & BD_NO_UMASK0)){
        umask(0);
    }

    if(!(flags & BD_NO_CHDIR)){
        chdir("/");
    }

    if(!(flags & BD_NO_CLOSE_FILES)){
        int maxfd=sysconf(_SC_OPEN_MAX);
        if(maxfd==-1){
            maxfd=BD_MAX_CLOSE;
        }
        for(int fd=0;fd<maxfd;fd++){
            close(fd);
        }
    }

    if(!(flags & BD_NO_REOPEN_STD_FDS)){
        close(STDIN_FILENO);
        int fd=open("/dev/null",O_RDWR);
        if(fd!=STDIN_FILENO) return -1;
        if(dup2(STDIN_FILENO,STDOUT_FILENO)!=STDOUT_FILENO) return -1;
        if(dup2(STDIN_FILENO,STDERR_FILENO)!=STDERR_FILENO) return -1;
    }
    return 0;
}