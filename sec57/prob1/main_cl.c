#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#define SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100

/*
例57-4を参考にする

実行すると途中で止まった
クライアント側
~~~~~
i=276
buf=000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000276

i=277
buf=000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000277

i=278
buf=000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000278

i=279
buf=000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000279

サーバー側
約1秒ごとに1行出力していた
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec57/prob1$ ./sv
000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001
000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002

しばらく待つと、サーバー側が200行ほど出力した後に、クライアント側が488まで出力した
i=486
buf=000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000486

i=487
buf=000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000487

i=488
buf=000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000488
*/

int main() {
    int sfd=socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family=AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);

    if(connect(sfd,(struct sockaddr *)&addr, sizeof(struct sockaddr_un))==-1){
        perror("connect");
        return 1;
    }

    ssize_t numRead;
    char buf[BUF_SIZE];
    for(int i=0;i<1000000;i++){
        printf("i=%d\n",i);
        sprintf(buf, "%.99d\n", i); // 改行含めて100文字になるようにする
        printf("buf=%s\n",buf);

        if(write(sfd, buf, strlen(buf))!=strlen(buf)){
            perror("partial/failed write");
            return 1;
        }
    }

    if(numRead==-1){
        perror("read");
        return 1;
    }

    return 0;
}