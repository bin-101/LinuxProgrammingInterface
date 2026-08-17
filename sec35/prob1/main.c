/* AI-generated code */
/* コマンド仕様は同じディレクトリの nice.man を参照。 */
#define _DEFAULT_SOURCE

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(const char *program)
{
    fprintf(stderr, "Usage: %s [-n adjustment] [command [arg ...]]\n", program);
    exit(EXIT_FAILURE);
}

static int parse_adjustment(const char *text)
{
    char *end;
    long value;

    /* 文字列全体を int の範囲の整数として読み取る。 */
    errno = 0;
    value = strtol(text, &end, 10);
    if (errno != 0 || *text == '\0' || *end != '\0' ||
        value < INT_MIN || value > INT_MAX) {
        fprintf(stderr, "Invalid adjustment: %s\n", text);
        exit(EXIT_FAILURE);
    }

    return (int)value;
}

int main(int argc, char *argv[])
{
    int adjustment = 10;  /* nice.man: "add integer N to the niceness (default 10)" */
    int option;

    /* nice.man: "-n, --adjustment=N" */
    while ((option = getopt(argc, argv, "+n:")) != -1) {
        if (option != 'n')
            usage(argv[0]);
        adjustment = parse_adjustment(optarg);
    }

    /* nice.man: "With no COMMAND, print the current niceness." */
    if (optind == argc) {
        if (argc == 1) {
            adjustment = 0;
        }

        /* nice() は成功時にも -1 を返し得るため errno も確認する。 */
        errno = 0;
        int new_nice = nice(adjustment);
        if (new_nice == -1 && errno != 0) {
            fprintf(stderr, "nice: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        printf("%d\n", new_nice);
        return EXIT_SUCCESS;
    }

    /* nice.man: "Run COMMAND with an adjusted niceness" */
    errno = 0;
    if (nice(adjustment) == -1 && errno != 0) {
        fprintf(stderr, "nice: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // TLPI(p778): "fork() により作成した子プロセスは nice 値を受け継ぎ、また exec() しても維持します。"
    execvp(argv[optind], &argv[optind]);
    fprintf(stderr, "%s: %s\n", argv[optind], strerror(errno));
    return EXIT_FAILURE;
}
