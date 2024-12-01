#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>

#define SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100
#define BACKLOG 5

/*
- ファイルシステム上にソケットファイルを作成
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec57/prob2$ gcc file.c -o file
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec57/prob2$ ./file

別ターミナルで、
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec57/prob1$ lsof -c file
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec57/prob1$ lsof -c file
COMMAND   PID   USER   FD   TYPE             DEVICE SIZE/OFF     NODE NAME
file    21584 bin101  cwd    DIR              259,7     4096 27133093 /home/bin101/code/LinuxProgrammingInterface/sec57/prob2
file    21584 bin101  rtd    DIR              259,7     4096        2 /
file    21584 bin101  txt    REG              259,7    16368 27133116 /home/bin101/code/LinuxProgrammingInterface/sec57/prob2/file
file    21584 bin101  mem    REG              259,7  2220400  2361462 /usr/lib/x86_64-linux-gnu/libc.so.6
file    21584 bin101  mem    REG              259,7   240936  2361448 /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
file    21584 bin101    0u   CHR              136,1      0t0        4 /dev/pts/1
file    21584 bin101    1u   CHR              136,1      0t0        4 /dev/pts/1
file    21584 bin101    2u   CHR              136,1      0t0        4 /dev/pts/1
file    21584 bin101    3u  unix 0x0000000000000000      0t0   146064 /tmp/us_xfr type=STREAM # ソケットファイル
file    21584 bin101   42w   REG              259,7      975 27132735 /home/bin101/.config/Code/logs/20241126T223110/ptyhost.log
file    21584 bin101   43u  unix 0x0000000000000000      0t0    25951 type=STREAM
file    21584 bin101   44u   CHR                5,2      0t0       90 /dev/ptmx
file    21584 bin101  103r   REG                7,2   666177    10882 /snap/code/174/usr/share/code/v8_context_snapshot.bin

- Linuxの抽象ソケット名前空間を使用

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec57/prob2$ gcc main_sv.c -o sv
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec57/prob2$ ./sv

別ターミナルで、
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ lsof -c sv
COMMAND   PID   USER   FD   TYPE             DEVICE SIZE/OFF     NODE NAME
sv      23157 bin101  cwd    DIR              259,7     4096 27133093 /home/bin101/code/LinuxProgrammingInterface/sec57/prob2
sv      23157 bin101  rtd    DIR              259,7     4096        2 /
sv      23157 bin101  txt    REG              259,7    16496 27133130 /home/bin101/code/LinuxProgrammingInterface/sec57/prob2/sv
sv      23157 bin101  mem    REG              259,7  2220400  2361462 /usr/lib/x86_64-linux-gnu/libc.so.6
sv      23157 bin101  mem    REG              259,7   240936  2361448 /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
sv      23157 bin101    0u   CHR              136,1      0t0        4 /dev/pts/1
sv      23157 bin101    1u   CHR              136,1      0t0        4 /dev/pts/1
sv      23157 bin101    2u   CHR              136,1      0t0        4 /dev/pts/1
sv      23157 bin101    3u  unix 0x0000000000000000      0t0   163976 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ type=STREAM
sv      23157 bin101   42w   REG              259,7      975 27132735 /home/bin101/.config/Code/logs/20241126T223110/ptyhost.log
sv      23157 bin101   43u  unix 0x0000000000000000      0t0    25951 type=STREAM
sv      23157 bin101   44u   CHR                5,2      0t0       90 /dev/ptmx
sv      23157 bin101  103r   REG                7,2   666177    10882 /snap/code/174/usr/share/code/v8_context_snapshot.bin

file    21584 bin101    3u  unix 0x0000000000000000      0t0 までは、プロセスID以外全部同じだが、NAMEが変わっていることを確認できた
*/

int main() {
    int sfd=socket(AF_UNIX, SOCK_STREAM, 0);

    if(remove(SOCK_PATH)==-1 && errno!=ENOENT){ // ENOENT: No such file or directory
        perror("remove");
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family=AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);

    if(bind(sfd,(struct sockaddr *)&addr,sizeof(struct sockaddr_un))==-1){
        perror("bind");
    }

    if(listen(sfd,BACKLOG)==-1){
        perror("listen");
    }
    pause();
}