#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/stat.h>
#define _GNU_SOURCE

/*
実行結果
```shell
~~(略)~~
0x7f309f2e1a60 0x7f309f2e1a60
root root
bin101 bin101
0x7f33ce400a60 0x7f33ce400a60
```

getpwuid()は(同じプログラム内の場合?)いつも同じポインタを返すため。
*/

int main(int argc, char *argv[])
{
    struct passwd *pwd;
    
    while((pwd=getpwent())!=NULL){
        printf("%s\n",pwd->pw_name);
        printf("%d\n",pwd->pw_uid);
    }
    endpwent();

    unsigned int uid1=0;
    unsigned int uid2=1000;
    printf("%p %p\n",getpwuid(uid1),getpwuid(uid2));
    printf("%s %s\n",getpwuid(uid1)->pw_name,getpwuid(uid2)->pw_name);
    printf("%s %s\n",getpwuid(uid2)->pw_name,getpwuid(uid1)->pw_name);

    printf("%p %p\n",getpwuid(uid1),getpwuid(uid2));
}