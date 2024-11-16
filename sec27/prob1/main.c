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

int main(int argc, char *argv[]){
    execlp(argv[1],argv[1],"hello world",(char *) NULL);
}