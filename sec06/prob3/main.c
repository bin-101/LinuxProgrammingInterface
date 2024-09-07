#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/*
getenv(): 
putenv(): 

setenv(char *name,char *value,int overwrite)
    putenv(name=value)
    getenvで存在するかしないかを判定できるのでそれを使う
unsetenv(const char *name)
    putenv(name)
    getenvで存在するかしないかを判定できるのでそれを使う
*/

extern char **environ;

int my_setenv(const char *name,const char *value,int overwrite)
{
    char *name_value;
    snprintf(name_value,256,"%s=%s",name,value);
    printf("%s\n",name_value);
    if(overwrite){
        int ret=putenv(name_value);
        if(ret!=0){
            return ret=-1;
        }
        return ret;
    }
    if(getenv(name)==NULL){ //環境にnameが存在しない
        int ret=putenv(name_value);
        if(ret!=0){
            ret=-1;
        }
        return ret;
    }
    printf("xxxxxxxx\n");
    // overwrite=0で環境にnameが存在する場合
    return -1;
}

int my_unsetenv(char *name)
{
    if(getenv(name)==NULL){ //環境にnameが存在しない
        return -1;
    }
    int ret=putenv(name);
    if(ret!=0){
        ret=-1;
    }
    return ret;
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
    printf("-1\n");
    printf("setenv\n");
    print_env();
    my_unsetenv(name);
    printf("\nunsetenv\n\n");
    print_env();
    return 0;
}
