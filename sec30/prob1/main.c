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

static volatile int glob=0;

struct threadInfo{
    pthread_t tid;
    int loops;
};

static void * threadFunc(void *arg){
    struct threadInfo *info=(struct threadInfo *) arg;
    int loops=info->loops;
    int loc;
    for(int j=0;j<loops;j++){
        loc=glob;
        loc++;
        glob=loc;
        printf("Thread %ld: %d\n",info->tid, glob);
    }
    return NULL;
}

int main(int argc,char *argv[]){
    int loops=1e7;
    if(argc>1){
        loops=atoi(argv[1]);
    }
    struct threadInfo tinfo1,tinfo2;
    tinfo1.loops=loops;
    tinfo2.loops=loops;
    pthread_create(&tinfo1.tid,NULL,threadFunc,&tinfo1);
    pthread_create(&tinfo2.tid,NULL,threadFunc,&tinfo2);
    pthread_join(tinfo1.tid,NULL);
    pthread_join(tinfo2.tid,NULL);

    printf("glob=%d\n",glob);
}

/*
実行結果の最初の部分を抜粋
Thread 139826122720832: 1
Thread 139826122720832: 3
Thread 139826122720832: 4
Thread 139826122720832: 5
Thread 139826122720832: 6
Thread 139826122720832: 7
Thread 139826122720832: 8
Thread 139826122720832: 9
Thread 139826122720832: 10
Thread 139826122720832: 11
Thread 139826122720832: 12
Thread 139826122720832: 13
Thread 139826122720832: 14
Thread 139826122720832: 15
Thread 139826122720832: 16
Thread 139826122720832: 17
Thread 139826122720832: 18
Thread 139826122720832: 19
Thread 139826122720832: 20
Thread 139826122720832: 21
Thread 139826122720832: 22
Thread 139826122720832: 23
Thread 139826122720832: 24
Thread 139826122720832: 25
Thread 139826122720832: 26
Thread 139826122720832: 27
Thread 139826122720832: 28
Thread 139826122720832: 29
Thread 139826122720832: 30
Thread 139826122720832: 31
Thread 139826122720832: 32
Thread 139826122720832: 33
Thread 139826122720832: 34
Thread 139826122720832: 35
Thread 139826122720832: 36
Thread 139826122720832: 37
Thread 139826112235072: 2
Thread 139826112235072: 39
Thread 139826112235072: 40
Thread 139826112235072: 41
Thread 139826112235072: 42
Thread 139826112235072: 43
Thread 139826112235072: 44
Thread 139826112235072: 45
Thread 139826112235072: 46
Thread 139826112235072: 47
Thread 139826112235072: 48
Thread 139826112235072: 49
Thread 139826112235072: 50
Thread 139826112235072: 51
Thread 139826112235072: 52
Thread 139826112235072: 53
Thread 139826112235072: 54
Thread 139826112235072: 55
Thread 139826112235072: 56
Thread 139826112235072: 57
Thread 139826112235072: 58
Thread 139826112235072: 59
Thread 139826112235072: 60
Thread 139826112235072: 61
Thread 139826112235072: 62
Thread 139826112235072: 63
Thread 139826112235072: 64
Thread 139826112235072: 65
Thread 139826122720832: 38
*/