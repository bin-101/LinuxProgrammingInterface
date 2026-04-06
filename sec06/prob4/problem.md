# 6-4

## 問題
`/proc/PID/environ` はプロセスの初期環境変数（プロセス起動時の状態）を出力することを確かめなさい。

## 解説
`man proc_pid_environ`の説明から一部を抜粋した。
```
This file contains the initial environment that was set when the currently executing program was started via execve(2).
```

## 検証
```bash
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ echo $$ # 現在のプロセスIDを表示
118778
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ wc -c /proc/118778/environ
3062 /proc/118778/environ
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ export TEST=test # 環境変数を追加
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ wc -c /proc/118778/environ
3062 /proc/118778/environ　
```

## 初期環境を出力する理由
環境変数はユーザー空間で管理されているのが理由？

[clearenv() の実装](../../.tours/6-4-clearenv.tour)