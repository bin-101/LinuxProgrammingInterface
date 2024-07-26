#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#define _GNU_SOURCE

/*
例29-1を参考にした

pthread_joinは指定したスレッドの終了を待つ関数
自分の自身の終了を待つことになってしまう
よって、プログラムは無限に停止する......と思ったが、普通に動作した

実行結果
```
main pid: 1127580
main before
threadFunc pid: 1127580
threadFunc before: Hello world
y: 35
threadFunc after : Hello world
x: 0
main after
```
一部のデッドロックを検知してくれるらしい
参考: https://stackoverflow.com/questions/24094855/why-does-pthread-join-not-create-a-deadlock

pthread_join(pthread_self(), NULL);のような実行を防ぐにはどうしたらいいか？
    pthread_equal(tid,pthread_self())を使い、自身のスレッドIDと等しいかを確認すれば良さそう。
*/

static void *threadFunc(void *arg)
{
    printf("threadFunc pid: %d\n", getpid());
    char *s = (char *)arg;
    printf("threadFunc before: %s\n", s);
    int y = pthread_join(pthread_self(), NULL);
    printf("y: %d\n", y);
    printf("threadFunc after : %s\n", s);
    return (void *)strlen(s);
}

int main(int argc, char *argv[])
{
    pthread_t t1;
    void *res;
    printf("main pid: %d\n", getpid());
    printf("main before\n");
    int s = pthread_create(&t1, NULL, threadFunc, "Hello world");
    int x = pthread_join(t1, &res);
    printf("x: %d\n", x);
    printf("main after\n");
}
