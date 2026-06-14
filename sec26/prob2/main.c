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
#include <sys/prctl.h>

void parent_handler(int sig){
    printf("Parent process received Signal %d (%s)\n",sig,strsignal(sig));
}
void child_handler(int sig){
    printf("Child process received Signal %d (%s)\n",sig,strsignal(sig));
}
void grand_child_handler(int sig){
    printf("Grand child process received Signal %d (%s)\n",sig,strsignal(sig));
}

void output_getppid(){
    printf("getppid(): %d\n",getppid());
}

void wait_signal(int signal_id, void (*handler)(int)){
    struct sigaction sa;
    sa.sa_handler=handler;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    if(sigaction(signal_id, &sa, NULL)==-1){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    // signal_idのシグナルが来るまで待つ sigsuspend
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, signal_id);
    if(sigsuspend(&mask)==-1 && errno!=EINTR){
        perror("sigsuspend");
        exit(EXIT_FAILURE);
    }
}

// nullシグナルを使用する
void wait_process_death(pid_t pid){
    while(kill(pid, 0)==0 || errno!=ESRCH){ // 親プロセスが存在するか確認
        sleep(1);
    }
}

void func_grand_child_pid(pid_t parent_pid,pid_t child_pid){
    printf("Grand child process started PID: %d\n", getpid());
    prctl(PR_SET_PDEATHSIG, SIGUSR1);
    printf("Grand child executed prctl\n");
    kill(getppid(), SIGUSR1); 
    wait_signal(SIGUSR1, grand_child_handler);
    printf("Child process exited\n");
    output_getppid();
    // wait_process_death(child_pid); コメントアウトすると無限に停止する。このことから、ゾンビプロセスが存在すると、nullシグナルを送ってもESRCHエラーが返らないことがわかる。
    kill(parent_pid, SIGUSR1); // 親プロセスにシグナルを送る
    wait_process_death(child_pid); // 子プロセスの死を待つ
    printf("Child process died\n");
    output_getppid();
}

void func_child_pid(pid_t parent_pid,pid_t child_pid){
    printf("Child process started PID: %d\n", getpid());
    pid_t grand_child_pid;
    switch(grand_child_pid=fork()){
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: // 孫プロセス
            func_grand_child_pid(parent_pid, child_pid);
            _exit(EXIT_SUCCESS);
        default: // 子プロセス
            wait_signal(SIGUSR1, child_handler);
            printf("Child process exiting\n");
            _exit(EXIT_SUCCESS);
    }
}

void block_all_signals(){
    sigset_t mask;
    sigfillset(&mask);
    if(sigprocmask(SIG_SETMASK, &mask, NULL)==-1){
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }   
}

int main(int argc,char *argv[]){
    block_all_signals();

    setbuf(stdout, NULL); // 出力のバッファリングを無効化

    printf("Parent process started PID: %d\n", getpid());

    pid_t child_pid;
    switch(child_pid=fork()){
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: // 子プロセス
            func_child_pid(getppid(), getpid());
            _exit(EXIT_SUCCESS);
        default: // 親プロセス
            break;
    }
    wait_signal(SIGUSR1, parent_handler);
    printf("Parent process waiting for child to exit\n");
    wait(NULL); // 子プロセスの終了を待つ
}