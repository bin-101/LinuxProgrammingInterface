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

struct my_pthread_once_t{
    _Bool is_used;
    pthread_mutex_t mutex;
};

struct my_pthread_once_t control = {0, PTHREAD_MUTEX_INITIALIZER};

int my_pthread_once(struct my_pthread_once_t *once_control, void (*init)(void)){
    int s=pthread_mutex_lock(&control.mutex);
    if(s){
        return s;
    }
    if(!control.is_used){
        (*init)();
        control.is_used = 1;
    }
    s=pthread_mutex_unlock(&control.mutex);
    if(s){
        return s;
    }
    return 0;
}

void init_func(){
    printf("Called init_func()\n");
}

void* threadFunc(void *arg){
    printf("%ld\n",pthread_self());
    my_pthread_once(&control,init_func);
    return NULL;
}

int main(){
    pthread_t t1,t2;
    pthread_create(&t1,NULL,threadFunc,NULL);
    pthread_create(&t2,NULL,threadFunc,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
}