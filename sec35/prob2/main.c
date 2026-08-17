/* AI-generated code */
/* APIの仕様は sched_setscheduler.man と setuid.man を参照。 */

#include <errno.h>
#include <limits.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static void usage(const char *program)
{
    fprintf(stderr,
            "Usage: %s {r|f} priority command [arg ...]\n",
            program);
    exit(EXIT_FAILURE);
}

static int parse_priority(const char *text)
{
    char *end;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);
    if (errno != 0 || *text == '\0' || *end != '\0' ||
        value < INT_MIN || value > INT_MAX) {
        fprintf(stderr, "Invalid priority: %s\n", text);
        exit(EXIT_FAILURE);
    }

    return (int)value;
}

int main(int argc, char *argv[])
{
    int policy;
    int priority;
    int min_priority;
    int max_priority;
    uid_t real_uid;
    struct sched_param param;

    if (argc < 4 || strlen(argv[1]) != 1 ||
        (argv[1][0] != 'r' && argv[1][0] != 'f')) {
        usage(argv[0]);
    }

    /* sched_setscheduler.man: "SCHED_RR a round-robin policy." */
    /* sched_setscheduler.man: "SCHED_FIFO a first-in, first-out policy" */
    policy = argv[1][0] == 'r' ? SCHED_RR : SCHED_FIFO;
    priority = parse_priority(argv[2]);

    min_priority = sched_get_priority_min(policy);
    max_priority = sched_get_priority_max(policy);
    if (min_priority == -1 || max_priority == -1) {
        fprintf(stderr, "sched_get_priority: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    /* sched_setscheduler.man: "a number in the range returned by calling"
       sched_get_priority_min(2) and sched_get_priority_max(2). */
    if (priority < min_priority || priority > max_priority) {
        fprintf(stderr, "Priority must be between %d and %d\n",
                min_priority, max_priority);
        return EXIT_FAILURE;
    }

    param.sched_priority = priority;
    real_uid = getuid();

    /* 実効UIDがrootの間にリアルタイムスケジューリングを設定する。 */
    if (sched_setscheduler(0, policy, &param) == -1) {
        fprintf(stderr, "sched_setscheduler: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    /* setuid.man: "all process-related user ID's are set to uid." */
    if (setuid(real_uid) == -1) {
        fprintf(stderr, "setuid: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    execvp(argv[3], &argv[3]);
    fprintf(stderr, "%s: %s\n", argv[3], strerror(errno));
    return EXIT_FAILURE;
}
