#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#define _GNU_SOURCE

struct passwd *my_getpwnam(const char *name)
{
    struct passwd *pwd;
    while((pwd=getpwent())!=NULL){
        if(strcmp(pwd->pw_name,name)==0){
            endpwent();
            return pwd;
        }
    }
    endpwent();
    return NULL;
}

int main(int argc, char *argv[])
{
    char *name="bin101";
    struct passwd *pwd=my_getpwnam(name);
    printf("%s %s %d %s\n",pwd->pw_name,pwd->pw_name,pwd->pw_uid,pwd->pw_shell);
}