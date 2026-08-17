# 35-1

`nice(1)` の簡易実装。
コマンド仕様は `man nice` の出力を保存した `nice.man` を参照。

```sh
gcc -Wall -Wextra -Werror -std=c11 main.c -o nice
./nice -n 5 sh -c 'ps -o ni= -p $$'
```

`nice()` で現在の nice 値に加算し、`execvp()` でコマンドを実行する。
nice 値は `exec()` 後も維持される。

## 実行結果

```console
$ ./nice
0

$ ./nice sh -c 'printf "nice value: "; ps -o ni= -p $$'
nice value:  10

$ ./nice -n 5 sh -c 'printf "nice value: "; ps -o ni= -p $$'
nice value:   5

$ ./nice -n 5 sh -c 'printf "argument: %s, nice value: " "$1"; ps -o ni= -p $$' _ hello
argument: hello, nice value:   5

$ ./nice -n -1 true
nice: Operation not permitted
```

`/usr/bin/nice` でもデフォルトは `10`、`-n 5` は `5` となり、
自作版と一致した。負の加算は優先度を上げる操作なので、通常ユーザーでは失敗する。
