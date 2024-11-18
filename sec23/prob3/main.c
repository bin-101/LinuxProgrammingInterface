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
#include <time.h>

/*
- evpがNULLの場合
    - sigev_notify: SIGEV_SIGNAL
    - sigev_signo: SIGALRM
    - sigev_value.sival_int: タイマID
*/

void handler(int sig, siginfo_t *infop, void *uc){
    printf("si_signo: %d\n", infop->si_signo); // シグナル番号
    printf("si_code: %d\n", infop->si_code); // シグナルコード
    printf("si_value.sival_int: %d\n", infop->si_value.sival_int);
    printf("si_value.sival_ptr: %p\n", infop->si_value.sival_ptr);
}

int main(){
    struct sigaction sa;
    sa.sa_sigaction=handler;
    sa.sa_flags=SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGALRM, &sa, NULL)==-1){
        printf("sigaction: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    timer_t timerid;
    if(timer_create(CLOCK_REALTIME, NULL, &timerid)==-1){
        printf("timer_create: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct itimerspec new_value;
    new_value.it_interval.tv_sec=0;
    new_value.it_interval.tv_nsec=0;
    new_value.it_value.tv_sec=1;
    new_value.it_value.tv_nsec=0;
    timer_settime(timerid, 0, &new_value, NULL);
    pause();

    printf("\n");

    struct sigevent evp;
    evp.sigev_notify=SIGEV_SIGNAL;
    evp.sigev_signo=SIGALRM;
    evp.sigev_value.sival_int=0; //先ほどと同じにするために0を代入
    if(timer_create(CLOCK_REALTIME, &evp, &timerid)==-1){
        printf("timer_create: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    timer_settime(timerid, 0, &new_value, NULL);
    pause();
}