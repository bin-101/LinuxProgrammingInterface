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
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <mqueue.h>
#include <semaphore.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec53/prob2$ ./53_1 -c /demo 0 # セマフォ作成
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec53/prob2$ ./53_3 /demo & # バックグラウンドで無限に待つ
[4] 137852
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec53/prob2$ ./53_4 /demo & # +1をする
[5] 137925
137852 sem_wait() succeeded
[4]   終了                  ./53_3 /demo
[5]   終了                  ./53_4 /demo
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec53/prob2$ time ./53_3timed /demo 5 # 5秒間待つ
sem_wait: Connection timed out
137998 sem_wait() succeeded

real    0m5.005s
user    0m0.001s
sys     0m0.003s
*/

int main(int argc,char *argv[]){
    sem_t *sem=sem_open(argv[1],0);
    if(sem==SEM_FAILED){
        perror("sem_open");
        exit(1);
    }
    struct timespec abs_timeout;
    if(clock_gettime(CLOCK_REALTIME,&abs_timeout)==-1){ 
        perror("clock_gettime");
        exit(1);
    }
    abs_timeout.tv_sec+=atoi(argv[2]);

    if(sem_timedwait(sem,&abs_timeout)==-1){
        perror("sem_wait");
    }
    printf("%ld sem_wait() succeeded\n",(long)getpid());
}
