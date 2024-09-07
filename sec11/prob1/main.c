#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#define _GNU_SOURCE

// 例11-1 丸写し
/*
実行結果
_SC_ARG_MAX:  2097152
_SC_LOGIN_NAME_MAX:  256
_SC_OPEN_MAX:  1048576
_SC_NGROUPS_MAX:  65536
_SC_PAGESIZE:  4096
_SC_RTSIG_MAX:  32

getconf ARG_MAX
2097152
*/
static void sysconfPrint(const char *msg, int name){
    long lim;

    errno = 0;
    lim = sysconf(name);
    if (lim != -1){
        printf("%s %ld\n", msg, lim);
    } else{
        if (errno == 0){
            printf("%s (indeterminate)\n",msg);
        }else{
            assert(0);
        }
    }
}

void main(){
    sysconfPrint("_SC_ARG_MAX: ", _SC_ARG_MAX);
    sysconfPrint("_SC_LOGIN_NAME_MAX: ", _SC_LOGIN_NAME_MAX);
    sysconfPrint("_SC_OPEN_MAX: ", _SC_OPEN_MAX);
    sysconfPrint("_SC_NGROUPS_MAX: ", _SC_NGROUPS_MAX);
    sysconfPrint("_SC_PAGESIZE: ", _SC_PAGESIZE);
    sysconfPrint("_SC_RTSIG_MAX: ", _SC_RTSIG_MAX);
}