// teeコマンドを実装する
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int i, opt;

    opterr = 0; // getopt()のエラーメッセージを無効にする。
    int ex_a = 0;

    while ((opt = getopt(argc, argv, "a")) != -1)
    {
        // コマンドライン引数のオプションがなくなるまで繰り返す
        switch (opt)
        {
        case 'a':
            printf("-aがオプションとして渡されました\n");
            ex_a = 1;
            break;

        default: /* '?' */
            // 指定していないオプションが渡された場合
            printf("Usage: %s [-f] [-g] [-h argment] arg1 ...\n", argv[0]);
            break;
        }
    }

    // オプション以外の引数を出力する
    char *file_name = argv[optind];
    printf("optind = %s\n", file_name);
    int fd = -1;
    if (ex_a == 0)
    {
        fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    }
    else
    {
        fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0777);
    }

    // 標準入力から最大100文字読み取る
    const int BUFFER_SIZE = 101;
    char buffer[BUFFER_SIZE];
    ssize_t numRead = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
    buffer[numRead] = '\0';
    write(STDOUT_FILENO, buffer, numRead);
    write(fd, buffer, numRead);

    close(fd);

    return 0;
}