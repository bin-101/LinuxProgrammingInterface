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
#include <time.h>

mode_t get_umask(mode_t mode){
    mode_t old_mask=umask(0);
    umask(old_mask);
    return old_mask;
}

int main(){
    mode_t mask=get_umask(0);
    printf("%d\n",mask);
    return 0;
}