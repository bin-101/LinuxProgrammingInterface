# 35-4

pipeの通信速度を、親子が同じCPUの場合と別CPUの場合で比較する。

```sh
gcc -Wall -Wextra -Werror -std=c11 main.c -o pipe_affinity
./pipe_affinity          # 512 MiBを転送
./pipe_affinity 1024     # 1 GiBを転送
```

親が書き手、子が読み手になる。計測前に同期し、
`sched_setaffinity()` でそれぞれのCPUを固定する。

結果はCPUのキャッシュ構成やハイパースレッディングによって変わる。
CPU構成は `lscpu -e` などで確認できる。

## 実行結果

```console
$ ./pipe_affinity
transfer: 512 MiB
same CPU       writer=0 reader=0: 0.165 s, 3106.5 MiB/s
different CPUs writer=0 reader=1: 0.206 s, 2481.8 MiB/s
same CPU is 1.25x faster
```

`writer` は親、`reader` は子が固定された論理CPU番号。
1行目は両方をCPU 0、2行目は親をCPU 0、子をCPU 1に固定している。
秒数は小さいほど、`MiB/s` は大きいほど高速になる。

3回実行した結果:

```text
回  same CPU  different CPUs  比率
1   3106.5    2481.8          1.25x
2   3372.2    2753.7          1.22x
3   3255.6    2653.4          1.23x
```

3回とも同じCPUの方が速く、1.22倍から1.25倍になった。

```console
$ lscpu -e=CPU,CORE,SOCKET,ONLINE
CPU CORE SOCKET ONLINE
  0    0      0    yes
  1    0      0    yes
```

CPU 0と1は別の論理CPUだが、同じ物理コア0を共有している。
このような構成も含め、結果はCPU構成によって変化する。

API仕様は次のファイルを参照。

- `sched_setaffinity.man`
- `pipe.man`
- `clock_gettime.man`
