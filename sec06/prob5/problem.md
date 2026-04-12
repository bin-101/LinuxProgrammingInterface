# 6-5

`/proc/self` が自プロセスの PID を示すシンボリックリンクとして機能することを確かめなさい。
また、`getpid()` と同じアルゴリズム（`__task_pid_nr_ns()`）が使われていることをカーネルのソースコードから確認しなさい。

# コードツアー

ツアー: [6-5: getpid()](../../.tours/6-5-getpid.tour)

ツアー: [6-5: /proc/self](../../.tours/6-5-proc-self.tour)
