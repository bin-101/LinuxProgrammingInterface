#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/*
setenv(char *name,char *value,int overwrite)
    overwirte=0以外: putenv(name=value)
    overwirte=0: getenvで存在するかしないかを判定し、存在しなければputenv(name=value)
unsetenv(const char *name)
    getenvで存在するかしないかを判定し、存在すればputenv(name=value)
*/

extern char **environ;

int my_setenv(const char *name,const char *value,int overwrite)
{
    char name_value[1000];
    snprintf(name_value,sizeof(name_value),"%s=%s",name,value);
    if(overwrite){
        int ret=putenv(name_value);
        if(ret!=0){
            return -1;
        }
        return 0;
    }
    if(getenv(name)==NULL){ //環境にnameが存在しない
        int ret=putenv(name_value);
        if(ret!=0){
            return -1;
        }
        return 0;
    }
    // overwrite=0で環境にnameが存在する場合
    return 0;
}

int my_unsetenv(char *name)
{
    if(getenv(name)==NULL){ //環境にnameが存在しない
        return 0;
    }
    int ret=putenv(name);
    if(ret!=0){
        return -1;
    }
    return 0;
}

void print_env()
{
    char **ep=environ;
    while(*ep!=NULL){
        printf("%s\n",*ep);
        ep++;
    }
}

int main(int argc, char *argv[])
{
    print_env();
    char *name="name1";
    char *value="value1";
    
    my_setenv(name,value,1);
    printf("\nsetenv\n");
    print_env();

    my_unsetenv(name);
    printf("\nunsetenv\n\n");
    print_env();
}
