- catのオプションn: 行番号を表示
- 実行結果
```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec27/prob3$ ./a.out
     1  #!/bin/cat -n
     2  Hello World
```
- execl("./cat.sh","./cat.sh",(char *) NULL);で何が起こっているか
    - interpreter-path: /bin/cat
    - optional-arg: -n
    - script-path: ./cat.sh
    - arg: argv[0]はインタプリタに渡されないため、なにもなし
- よって、`/bin/cat -n ./cat.sh`を実行するのと同じ
```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec27/prob3$ /bin/cat -n ./cat.sh
     1  #!/bin/cat -n
     2  Hello World
```