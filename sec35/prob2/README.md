# 35-2

リアルタイムスケジューリング版 `nice(1)`。

```sh
gcc -Wall -Wextra -Werror -std=c11 main.c -o rtsched
sudo chown root:root rtsched
sudo chmod u+s rtsched
./rtsched r 10 command arg...
```

`sched_setscheduler()` は権限を落とす前に呼ぶ。
その後 `setuid(getuid())` で root 権限を完全に破棄してから
`execvp()` でコマンドを実行する。

API仕様は `sched_setscheduler.man` と `setuid.man` を参照。

通常権限では `sched_setscheduler()` が `EPERM` になることを確認済み。
set-user-ID-root の設定は行っていない。

## 実行結果

```console
$ ./rtsched
Usage: ./rtsched {r|f} priority command [arg ...]

$ ./rtsched r abc true
Invalid priority: abc

$ ./rtsched f 0 true
Priority must be between 1 and 99

$ ./rtsched r 1 true
sched_setscheduler: Operation not permitted

$ chrt -r 1 true
chrt: failed to set pid 0's policy: Operation not permitted
```

`r` は `SCHED_RR`、`f` は `SCHED_FIFO` を表す。この環境で使える
リアルタイム優先度は `1` から `99` まで。

現在の `rtsched` は通常ユーザー所有で、set-user-IDビットも付いていない。
そのため実効UIDはrootにならず、リアルタイム方針の設定が `EPERM` で失敗する。
標準の `chrt` でも同じ結果になる。この時点で終了するため、後ろの `true` は
実行されない。

set-user-ID-rootで実行した場合は、まずroot権限でスケジューリング方針を設定し、
`setuid(getuid())` で呼び出し元ユーザーへ戻ってからコマンドを実行する。
