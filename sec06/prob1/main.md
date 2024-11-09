```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec06/prob1$ ls -lh a.out
-rwxrwxr-x 1 bin101 bin101 16K 11月  9 18:15 a.out
```
- 16K Byte
- 未初期化データセグメントにはプログラムロード時に領域を割り当て、実行前に0で初期化する。
プログラム自体にはディスク領域を割り当てる必要がない。