#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include "../../sec59/prob2/socket_library.h"

/*
同じソケットファイルディスクリプタにlistenを行った場合、ポート番号は変わらなかった。
違うソケットファイルディスクリプタにlistenを行った場合、ポート番号は変わった。
出力結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec61/prob4$ ./a.out
Port: 1
(?UNKNOWN?)
0
(?UNKNOWN?)
Port: 0
(?UNKNOWN?) # なぜか最初は出力が変になった。理由わからず
Port: 51319
(::, 51319)
51319
(::, 51319)
Port: 51319
(::, 51319) # netstat -a | grep 51319で次のように出力された tcp6       0      0 [::]:51319              [::]:*                  LISTEN
Port: 51319
(::, 51319)
51319
(::, 51319)
Port: 51319
(::, 51319)
Port: 51319
(::, 51319)
51319
(::, 51319)
Port: 51319
(::, 51319)
Port: 51319
(::, 51319)
51319
(::, 51319)
Port: 51319
(::, 51319)
Port: 51319
(::, 51319)
51319
(::, 51319)
Port: 51319
(::, 51319)
Port: 51319
(::, 51319)
51319
(::, 51319)
Port: 51319
(::, 51319)
Port: 51319
(::, 51319)
51319
(::, 51319)
Port: 51319
(::, 51319)
Port: 51319
(::, 51319)
51319
(::, 51319)
Port: 51319
(::, 51319)
Port: 51319
(::, 51319)
51319
(::, 51319)
Port: 51319
(::, 51319)
Port: 57715
(0.0.0.0, 57715)
57715
(0.0.0.0, 57715)
Port: 57715
(0.0.0.0, 57715)
Port: 46783
(0.0.0.0, 46783)
46783
(0.0.0.0, 46783)
Port: 46783
(0.0.0.0, 46783) # netstat -a | grep 46783で次のように出力された tcp        0      0 0.0.0.0:46783           0.0.0.0:*               LISTEN
Port: 40851
(0.0.0.0, 40851)
40851
(0.0.0.0, 40851)
Port: 40851
(0.0.0.0, 40851)
Port: 38875
(0.0.0.0, 38875)
38875
(0.0.0.0, 38875)
Port: 38875
(0.0.0.0, 38875)
Port: 57681
(0.0.0.0, 57681)
57681
(0.0.0.0, 57681)
Port: 57681
(0.0.0.0, 57681)
Port: 42371
(0.0.0.0, 42371)
42371
(0.0.0.0, 42371)
Port: 42371
(0.0.0.0, 42371)
Port: 44805
(0.0.0.0, 44805)
44805
(0.0.0.0, 44805)
Port: 44805
(0.0.0.0, 44805)
Port: 49569
(0.0.0.0, 49569)
49569
(0.0.0.0, 49569)
Port: 49569
(0.0.0.0, 49569)
Port: 59763
(0.0.0.0, 59763)
59763
(0.0.0.0, 59763)
Port: 59763
(0.0.0.0, 59763)
Port: 60553
(0.0.0.0, 60553)
60553
(0.0.0.0, 60553)
Port: 60553
(0.0.0.0, 60553)
*/

void print(int sfd){
    sleep(1);
    if(listen(sfd,10)==-1){
        printf("listen: %s\n",strerror(errno));
    }
    sleep(1);
    struct sockaddr_in6 s6;
    socklen_t s6_len;
    if(getsockname(sfd,(struct sockaddr *)&s6,&s6_len)==-1){
        printf("getsockname: %s\n",strerror(errno));
    }
    printf("Port: %d\n", ntohs(s6.sin6_port));
    char str[100];
    printf("%s\n",inetAddressStr((struct sockaddr *)&s6,s6_len,str,100));

    struct sockaddr_storage ss;
    socklen_t ss_len;
    getsockname(sfd,(struct sockaddr *)&ss,&ss_len);
    struct sockaddr_in6 *s6p=(struct sockaddr_in6 *)&ss;
    printf("%d\n",ntohs(s6p->sin6_port));
    printf("%s\n",inetAddressStr((struct sockaddr *)&ss,ss_len,str,100));

    struct sockaddr_in s4;
    socklen_t s4_len;
    if(getsockname(sfd,(struct sockaddr *)&s4,&s4_len)==-1){
        printf("getsockname: %s\n",strerror(errno));
    }
    printf("Port: %d\n", ntohs(s4.sin_port));
    str[100];
    printf("%s\n",inetAddressStr((struct sockaddr *)&s4,s4_len,str,100));
}

int main() {
    int sfd=socket(AF_INET6, SOCK_STREAM, 0);
    for(int i=0;i<10;i++){
        print(sfd);
        if(i==1) sleep(20);
    }
    for(int i=0;i<10;i++){
        int sfd=socket(AF_INET, SOCK_STREAM, 0);
        print(sfd);
        if(i==1) sleep(20);
    }
}