# 35-3

同じCPU・同じ `SCHED_FIFO` 優先度で親子を動かす。
各プロセスはCPU時間を3秒消費し、1秒ごとに `sched_yield()` する。

```sh
gcc -Wall -Wextra -Werror -std=c11 main.c -o demo_sched_fifo
sudo ./demo_sched_fifo
```

出力は `parent` 1秒、`child` 1秒の単位で交互になる。
暴走対策として各プロセスの `RLIMIT_CPU` を5秒に制限している。

API仕様は次のファイルを参照。

- `sched_setscheduler.man`
- `sched_setaffinity.man`
- `sched_yield.man`
- `times.man`
- `setrlimit.man`

通常権限では `sched_setscheduler()` が `EPERM` になることを確認済み。
`sudo` でのリアルタイム実行も成功した。

## 通常権限での実行結果

```console
$ taskset -pc $$
pid 2's current affinity list: 0-15

$ ulimit -r
0

$ ./demo_sched_fifo
cpu=0 policy=SCHED_FIFO priority=1
sched_setscheduler: Operation not permitted
```

`taskset` はプロセスのCPU affinityを確認・変更するコマンド。
`-p` はPIDを指定し、`-c` はCPUを一覧形式で表示する。`$$` は現在の
シェルのPIDなので、`taskset -pc $$` は「このシェルが使用できるCPU」を
表示する。子プロセスはこの設定を継承するため、プログラムが選べるCPUの
確認に使っている。

`cpu=0` は、使用可能なCPU `0-15` のうち最初の1つを選べたことを示す。
`priority=1` は `SCHED_FIFO` の最低優先度である。

この環境ではリアルタイム優先度の上限が `0` で、`CAP_SYS_NICE` もない。
そのため `sched_setscheduler()` が `EPERM` で失敗する。この時点ではまだ
`fork()` していないので、親子のCPU消費と交互実行は始まらない。

## sudoでの実行結果

```console
$ sudo ./demo_sched_fifo
cpu=0 policy=SCHED_FIFO priority=1
parent pid=66681 cpu=0.25
parent pid=66681 cpu=0.50
parent pid=66681 cpu=0.75
parent pid=66681 cpu=1.00
child  pid=66682 cpu=0.25
child  pid=66682 cpu=0.50
child  pid=66682 cpu=0.75
child  pid=66682 cpu=1.00
parent pid=66681 cpu=1.25
parent pid=66681 cpu=1.50
parent pid=66681 cpu=1.75
parent pid=66681 cpu=2.00
child  pid=66682 cpu=1.25
child  pid=66682 cpu=1.50
child  pid=66682 cpu=1.75
child  pid=66682 cpu=2.00
parent pid=66681 cpu=2.25
parent pid=66681 cpu=2.50
parent pid=66681 cpu=2.75
parent pid=66681 cpu=3.00
child  pid=66682 cpu=2.25
child  pid=66682 cpu=2.50
child  pid=66682 cpu=2.75
child  pid=66682 cpu=3.00
```

親と子がCPU時間1秒ごとに `sched_yield()` で交代し、それぞれ3秒で
終了している。設定前に `RLIMIT_CPU` を5秒へ制限しているため、
バグがあってもCPUを占有し続けない。
