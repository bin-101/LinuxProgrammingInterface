#define _GNU_SOURCE
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

/*
実行結果
```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec29/prob1$ ./a.out
y: 35
strerror(y): Resource deadlock avoided
error: 0 # エラーを起こした場合でも、Pthreds APIがerrnoに代入しないことを確かめられた
x: 0
```
pthread_joinは指定したスレッドの終了を待つ関数
自分の自身の終了を待つことになってしまう
よって、プログラムは無限に停止する......と思ったが、普通に動作した

一部のデッドロックを検知してくれるらしい
参考: https://stackoverflow.com/questions/24094855/why-does-pthread-join-not-create-a-deadlock

pthread_join(pthread_self(), NULL);のような実行を防ぐにはどうしたらいいか？
    pthread_equal(tid,pthread_self())を使い、自身のスレッドIDと等しいかを確認すれば良さそう。
*/

static void *threadFunc(void *arg)
{
    int y = pthread_join(pthread_self(), NULL);
    printf("y: %d\n", y);
    printf("strerror(y): %s\n", strerror(y));
    printf("error: %d\n", errno);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t1;
    int s = pthread_create(&t1, NULL, threadFunc, NULL);
    int x = pthread_join(t1, NULL);
    printf("x: %d\n", x);
}
