#define _GNU_SOURCE
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/*
準備
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec09/prob4$ gcc main.c
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec09/prob4$ sudo chown outsider ./a.out
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec09/prob4$ sudo chmod u+s ./a.out

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec09/prob4$ ./a.out 
a: seteuid
uid=1000 euid=1001 suid=1001
uid=1000 euid=1000 suid=1001
uid=1000 euid=1001 suid=1001
a: setreuid
uid=1000 euid=1001 suid=1001
uid=1000 euid=1000 suid=1001
uid=1000 euid=1001 suid=1001
b: setreuid
uid=1000 euid=1001 suid=1001
uid=1000 euid=1000 suid=1000
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec09/prob4$ ./a.out 0
a: seteuid
uid=1000 euid=1001 suid=1001
uid=1000 euid=1000 suid=1001
uid=1000 euid=1001 suid=1001
a: setreuid
uid=1000 euid=1001 suid=1001
uid=1000 euid=1000 suid=1001
uid=1000 euid=1001 suid=1001
b: setresuid
uid=1000 euid=1001 suid=1001
uid=1000 euid=1000 suid=1000
*/


void print_perm()
{
    uid_t uid, euid, suid;
    getresuid(&uid,&euid,&suid);
    printf("uid=%d euid=%d suid=%d\n",uid,euid,suid);
}

int main(int argc, char *argv[])
{
    // a
    printf("a: seteuid\n");
    print_perm();
    int old_euid=geteuid();
    seteuid(getuid());
    print_perm();
    seteuid(old_euid);
    print_perm();


    // a 
    printf("a: setreuid\n");
    print_perm();
    old_euid=geteuid();
    setreuid(-1, getuid()); // ruid=-1 かつ euid=getuid()のため、suidは変化しない
    print_perm();
    setreuid(-1, old_euid);
    print_perm();

    if(argc==2){
        // b
        printf("b: setresuid\n");
        print_perm();
        setresuid(-1, getuid(), getuid());
        print_perm();
    }else{
        // b
        printf("b: setreuid\n");
        print_perm();
        setreuid(getuid(), getuid()); // ruid=getuid()にしたため、suidも変化する
        print_perm();
    }
}
