/* AI-generated code */
/* APIの仕様は同じディレクトリの各 man ファイルを参照。 */

#define _GNU_SOURCE

#include <errno.h>
#include <limits.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_MIB 512ULL
#define MIB (1024ULL * 1024ULL)
#define BUFFER_SIZE (64 * 1024)

/* システムコールのエラーを表示して終了する。 */
static void die(const char *function)
{
    fprintf(stderr, "%s: %s\n", function, strerror(errno));
    exit(EXIT_FAILURE);
}

/* コマンドラインの転送量をバイト数へ変換する。 */
static uint64_t parse_size(const char *text)
{
    char *end;
    unsigned long long mib;

    errno = 0;
    mib = strtoull(text, &end, 10);
    if (errno != 0 || *text == '\0' || *text == '-' || *end != '\0' ||
        mib == 0 || mib > UINT64_MAX / MIB) {
        fprintf(stderr, "Invalid size in MiB: %s\n", text);
        exit(EXIT_FAILURE);
    }

    return (uint64_t)mib * MIB;
}

/* 現在のCPU affinityから、実験に使える2つのCPUを選ぶ。 */
static void find_two_cpus(cpu_set_t *original, int *first, int *second)
{
    if (sched_getaffinity(0, sizeof(*original), original) == -1)
        die("sched_getaffinity");

    *first = -1;
    *second = -1;

    /* affinityに含まれるCPUを番号の小さい順に2つ選ぶ。 */
    for (int cpu = 0; cpu < CPU_SETSIZE; cpu++) {
        if (!CPU_ISSET(cpu, original))
            continue;

        if (*first == -1)
            *first = cpu;
        else {
            *second = cpu;
            return;
        }
    }

    fprintf(stderr, "At least two available CPUs are required\n");
    exit(EXIT_FAILURE);
}

/* 指定した1つのCPUだけでプロセスを実行する。 */
static void pin_to_cpu(int cpu)
{
    cpu_set_t set;

    CPU_ZERO(&set);
    CPU_SET(cpu, &set);

    /* sched_setaffinity.man: "determines the set of CPUs on which it is eligible to run." */
    if (sched_setaffinity(0, sizeof(set), &set) == -1)
        die("sched_setaffinity");
}

/* pipeへ指定量のデータを書き切る。 */
static void send_data(int fd, uint64_t total_bytes)
{
    char buffer[BUFFER_SIZE] = {0};
    uint64_t sent = 0;

    while (sent < total_bytes) {
        size_t count = sizeof(buffer);
        ssize_t written;

        /* 最後の書き込みだけ残りバイト数に合わせる。 */
        if (total_bytes - sent < count)
            count = (size_t)(total_bytes - sent);

        /* write() は一部だけ書き込む場合がある。 */
        written = write(fd, buffer, count);
        if (written == -1) {
            if (errno == EINTR)
                continue;
            die("write");
        }
        if (written == 0) {
            errno = EIO;
            die("write");
        }

        sent += (uint64_t)written;
    }
}

/* EOFまでpipeを読み、受信した合計バイト数を返す。 */
static uint64_t receive_data(int fd)
{
    char buffer[BUFFER_SIZE];
    uint64_t received = 0;

    for (;;) {
        ssize_t count = read(fd, buffer, sizeof(buffer));

        /* 書き込み側が閉じられるとread()は0を返す。 */
        if (count == 0)
            return received;
        if (count == -1) {
            if (errno == EINTR)
                continue;
            die("read");
        }

        received += (uint64_t)count;
    }
}

/* 子がCPU固定を終えるまで親を待機させる。 */
static void wait_for_reader(int fd)
{
    char ready;
    ssize_t count;

    do {
        count = read(fd, &ready, 1);
    } while (count == -1 && errno == EINTR);

    if (count != 1) {
        if (count == 0)
            errno = EPIPE;
        die("reader synchronization");
    }
}

/* timespec間の経過秒数を計算する。 */
static double elapsed_seconds(const struct timespec *start,
                              const struct timespec *end)
{
    return end->tv_sec - start->tv_sec +
           (end->tv_nsec - start->tv_nsec) / 1000000000.0;
}

/* 指定したCPU配置で親から子へデータを送り、経過時間を返す。 */
static double run_case(int writer_cpu, int reader_cpu, uint64_t total_bytes)
{
    int data_pipe[2];
    int ready_pipe[2];
    struct timespec start;
    struct timespec end;
    pid_t child;
    int status;

    /* data_pipeは転送用、ready_pipeは計測開始の同期用。 */
    /* pipe.man: "a unidirectional data channel that can be used for interprocess communication." */
    if (pipe(data_pipe) == -1 || pipe(ready_pipe) == -1)
        die("pipe");

    child = fork();
    if (child == -1)
        die("fork");

    if (child == 0) {
        uint64_t received;
        char ready = 1;

        /* 子は読み手。不要なpipe端を閉じ、指定CPUへ移動する。 */
        close(data_pipe[1]);
        close(ready_pipe[0]);
        pin_to_cpu(reader_cpu);

        /* affinity設定完了を親へ通知してから受信を始める。 */
        if (write(ready_pipe[1], &ready, 1) != 1)
            die("reader synchronization");
        close(ready_pipe[1]);

        received = receive_data(data_pipe[0]);
        close(data_pipe[0]);
        if (received != total_bytes) {
            fprintf(stderr, "Received byte count does not match\n");
            _exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
    }

    /* 親は書き手。子の準備完了後から時間を測る。 */
    close(data_pipe[0]);
    close(ready_pipe[1]);
    pin_to_cpu(writer_cpu);
    wait_for_reader(ready_pipe[0]);
    close(ready_pipe[0]);

    /* clock_gettime.man: "not affected by discontinuous jumps in the system time" */
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
        die("clock_gettime");

    /* 全データを書き、write端を閉じて子へEOFを伝える。 */
    send_data(data_pipe[1], total_bytes);
    close(data_pipe[1]);

    /* 子が全データを読み終えるまでを計測範囲に含める。 */
    while (waitpid(child, &status, 0) == -1) {
        if (errno != EINTR)
            die("waitpid");
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
        die("clock_gettime");

    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS) {
        fprintf(stderr, "Reader terminated abnormally\n");
        exit(EXIT_FAILURE);
    }

    return elapsed_seconds(&start, &end);
}

int main(int argc, char *argv[])
{
    uint64_t total_bytes = DEFAULT_MIB * MIB;
    cpu_set_t original;
    int first_cpu;
    int second_cpu;
    double same_seconds;
    double different_seconds;
    double size_mib;
    double same_speed;
    double different_speed;

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [MiB]\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (argc == 2)
        total_bytes = parse_size(argv[1]);

    setvbuf(stdout, NULL, _IONBF, 0);
    signal(SIGPIPE, SIG_IGN);
    find_two_cpus(&original, &first_cpu, &second_cpu);

    /* 転送量を揃え、読み手のCPUだけを変えて比較する。 */
    same_seconds = run_case(first_cpu, first_cpu, total_bytes);
    different_seconds = run_case(first_cpu, second_cpu, total_bytes);

    /* 実験後は呼び出し時のCPU affinityへ戻す。 */
    if (sched_setaffinity(0, sizeof(original), &original) == -1)
        die("sched_setaffinity");

    size_mib = total_bytes / (double)MIB;

    /* MiB / 秒からスループットを求める。 */
    same_speed = size_mib / same_seconds;
    different_speed = size_mib / different_seconds;

    printf("transfer: %.0f MiB\n", size_mib);
    printf("same CPU       writer=%d reader=%d: %.3f s, %.1f MiB/s\n",
           first_cpu, first_cpu, same_seconds, same_speed);
    printf("different CPUs writer=%d reader=%d: %.3f s, %.1f MiB/s\n",
           first_cpu, second_cpu, different_seconds, different_speed);

    if (same_speed >= different_speed)
        printf("same CPU is %.2fx faster\n", same_speed / different_speed);
    else
        printf("different CPUs are %.2fx faster\n", different_speed / same_speed);

    return EXIT_SUCCESS;
}
