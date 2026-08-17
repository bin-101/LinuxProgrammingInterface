/* AI-generated code */
/* APIの仕様は同じディレクトリの各 man ファイルを参照。 */

#define _GNU_SOURCE

#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <unistd.h>

#define RUN_SECONDS 3
#define CPU_LIMIT_SECONDS 5

/* システムコールのエラーを表示して終了する。 */
static void die(const char *function)
{
    fprintf(stderr, "%s: %s\n", function, strerror(errno));
    exit(EXIT_FAILURE);
}

/* 使用を許可されたCPUから1つ選び、このプロセスを固定する。 */
static int pin_to_one_cpu(void)
{
    cpu_set_t allowed;
    cpu_set_t selected;

    if (sched_getaffinity(0, sizeof(allowed), &allowed) == -1)
        die("sched_getaffinity");

    for (int cpu = 0; cpu < CPU_SETSIZE; cpu++) {
        if (!CPU_ISSET(cpu, &allowed))
            continue;

        CPU_ZERO(&selected);
        CPU_SET(cpu, &selected);
        if (sched_setaffinity(0, sizeof(selected), &selected) == -1)
            die("sched_setaffinity");
        return cpu;
    }

    fprintf(stderr, "No available CPU\n");
    exit(EXIT_FAILURE);
}

/* CPUを3秒消費し、0.25秒ごとに進捗を表示する。 */
static void consume_cpu(const char *name)
{
    const long ticks_per_second = sysconf(_SC_CLK_TCK);
    struct tms start;
    struct tms now;
    clock_t start_ticks;
    int next_quarter = 1;

    if (ticks_per_second == -1)
        die("sysconf");

    /* times.man: "times() stores the current process times" */
    if (times(&start) == (clock_t)-1)
        die("times");

    /* 関数開始前に消費したCPU時間を計測対象から除く。 */
    start_ticks = start.tms_utime + start.tms_stime;

    while (next_quarter <= RUN_SECONDS * 4) {
        clock_t used_ticks;

        if (times(&now) == (clock_t)-1)
            die("times");

        /* ユーザーCPU時間とシステムCPU時間を合計する。 */
        used_ticks = now.tms_utime + now.tms_stime - start_ticks;

        /* 次の0.25秒に達するまでは計測を続ける。 */
        if (used_ticks * 4 < (clock_t)next_quarter * ticks_per_second)
            continue;

        printf("%-6s pid=%ld cpu=%.2f\n",
               name, (long)getpid(), used_ticks / (double)ticks_per_second);

        /* 1秒ごとに同じ優先度のもう一方へCPUを譲る。 */
        if (next_quarter % 4 == 0 && next_quarter < RUN_SECONDS * 4) {
            /* sched_yield.man: "moved to the end of the queue" */
            if (sched_yield() == -1)
                die("sched_yield");
        }

        next_quarter++;
    }
}

int main(void)
{
    const int cpu = pin_to_one_cpu();
    struct rlimit limit = {CPU_LIMIT_SECONDS, CPU_LIMIT_SECONDS};
    struct sched_param param;
    pid_t child;
    int status;

    setvbuf(stdout, NULL, _IONBF, 0);

    /* バグがあってもCPUを占有し続けないように上限を設ける。 */
    /* setrlimit.man: "a limit, in seconds, on the amount of CPU time" */
    if (setrlimit(RLIMIT_CPU, &limit) == -1)
        die("setrlimit");

    param.sched_priority = sched_get_priority_min(SCHED_FIFO);
    if (param.sched_priority == -1)
        die("sched_get_priority_min");

    printf("cpu=%d policy=SCHED_FIFO priority=%d\n",
           cpu, param.sched_priority);

    /* pid 0 は呼び出したプロセス自身を表す。設定は子にも継承される。 */
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1)
        die("sched_setscheduler");

    /* fork後は親子とも同じCPU・同じFIFO優先度で動く。 */
    child = fork();
    if (child == -1)
        die("fork");

    if (child == 0) {
        consume_cpu("child");
        _exit(EXIT_SUCCESS);
    }

    consume_cpu("parent");

    /* 子の終了を待ち、異常終了していないか確認する。 */
    if (waitpid(child, &status, 0) == -1)
        die("waitpid");

    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS) {
        fprintf(stderr, "Child terminated abnormally\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
