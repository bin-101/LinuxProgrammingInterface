- msgsndのmanページを見ると以下の記載があり、mtypeが正でなければならないことがわかる。
```
           struct msgbuf {
               long mtype;       /* message type, must be > 0 */
               char mtext[1];    /* message data */
           };
```
```
       EINVAL Invalid msqid value, or nonpositive mtype value, or invalid msgsz value (less than 0 or greater than the system value MSGMAX).
```

- msggetのmanページを見ると、メッセージキューのIDが0もあり得ることがわかる。
```
RETURN VALUE
       If successful, the return value will be the message queue identifier (a nonnegative integer), otherwise -1 with errno indicating the error.
```

よって、mtypeにメッセージキューのIDを代入できないことが分かった。
メッセージキューのIDは小さい(下に記載)ため、メッセージキューのID+1を代入するようにすればよさそう。
```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec46/prob3$ cat /proc/sys/kernel/msgmni
32000
```