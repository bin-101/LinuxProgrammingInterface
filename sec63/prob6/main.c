#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/epoll.h>

/*
manから引用
       Note that it is possible to call epoll_wait() on an epoll instance whose interest list is currently empty (or whose interest list becomes empty because file descriptors are closed or re‐
       moved  from  the  interest  in  another  thread).  The call will block until some file descriptor is later added to the interest list (in another thread) and that file descriptor becomes
       ready.
*/

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob6$ ./a.out
prepare end
k
res: 1
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec63/prob6$ k
k: コマンドが見つかりません
*/

struct epoll_event ev;
int epfd;

void* threadFunc(void *arg){
    sleep(5);
    ev.events=EPOLLIN;
    ev.data.fd=0;
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,0,&ev)==-1){
        perror("epoll_ctl");
    }
    printf("prepare end\n");
}




int main(int argc,char *argv[]){
    epfd=epoll_create(1);
    if(epfd==-1){
        perror("epoll_create");
        exit(1);
    }
    pthread_t t1;
    pthread_create(&t1,NULL,threadFunc,NULL);
    int res=epoll_wait(epfd,&ev,1,-1);
    printf("res: %d\n",res);
}