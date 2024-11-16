- 似た状況を作った
```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec27/prob1$ ls -l dir1 dir2
dir1:
合計 4
-rw-rw-r-- 1 bin101 bin101 23 11月 16 17:33 xyz

dir2:
合計 4
-rwxrwxr-x 1 bin101 bin101 23 11月 16 17:33 xyz

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec27/prob1$ echo $PATH
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin:./dir1:./dir2
```

- `execlp(argv[1],argv[1],"hello world",(char *) NULL);`を実行
```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec27/prob1$ ./a.out xyz
dir2
```
- ./dir1のほうが先にあるが、実行許可されていないため、dir2の方が実行される
    - (他のディレクトリにxyzファイルがないと仮定)