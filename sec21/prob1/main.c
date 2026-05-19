#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/*
参考(glibc): glibc/stdlib/abort.c

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec21/prob1$ ./a.out
raising SIGABRT
caught SIGABRT
raising SIGABRT again
中止 (コアダンプ)
*/

void my_abort(){
    // The abort() function first unblocks the SIGABRT signal,
    // and then raises that signal for the calling process (as though raise(3) was called).
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGABRT);
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
    printf("raising SIGABRT\n");
    raise(SIGABRT);

    // If the SIGABRT signal is ignored, or caught by a handler that returns, the abort() function will still terminate the process.  It does this by restoring the default dis‐
    // position for SIGABRT and then raising the signal for a second time.
    struct sigaction sa;
    sa.sa_handler=SIG_DFL;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGABRT, &sa, NULL);

    // シグナルブロックの設定が変更されている可能性を考慮して、再度解除
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);

    printf("raising SIGABRT again\n");
    raise(SIGABRT);

    // ここにはこない
}

void sigabrt_handler(int sig){
    printf("caught SIGABRT\n");
}

int main(int argc, char *argv[]) {

    struct sigaction sa;
    sa.sa_handler=sigabrt_handler;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGABRT, &sa, NULL);
    my_abort();
}