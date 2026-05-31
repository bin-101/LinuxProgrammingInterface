#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// TLPI 例24-5を写経

/*
macOSとLinuxで動作確認した。どちらもparentが先に出力された。

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec24/prob4$ uname -a
Linux bin101-Inspiron-16-5635 6.14.0-37-generic #37-Ubuntu SMP PREEMPT_DYNAMIC Fri Nov 14 22:10:32 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec24/prob4$ ./a.out 10
0 parent
0 child
1 parent
1 child
2 parent
2 child
3 parent
3 child
4 parent
4 child
5 parent
5 child
6 parent
6 child
7 parent
7 child
8 parent
8 child
9 parent
9 child

以下は、macOSでの実行結果
$ ./a.out 10
0 parent
0 child
1 parent
1 child
2 parent
2 child
3 parent
3 child
4 parent
4 child
5 parent
5 child
6 parent
6 child
7 parent
7 child
8 parent
8 child
9 parent
9 child
*/

int main(int argc, char *argv[])
{
    int numChildren, j;
    pid_t childPid;
    if(argc>1 && strcmp(argv[1], "--help") == 0){
        printf("%s num-children\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    numChildren = (argc > 1) ? atoi(argv[1]) : 1;
    setbuf(stdout, NULL);

    for (j = 0; j < numChildren; j++) {
        switch (childPid = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:             /* Child */
            printf("%d child\n", j);
            _exit(EXIT_SUCCESS);
        default:            /* Parent */
            printf("%d parent\n", j);
            wait(NULL);    /* Wait for child to terminate */
            break;
        }
    }
    return 0;
}   