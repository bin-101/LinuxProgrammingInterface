#include "socket_library.h"

#define PORT_NUM "50000"
#define INT_LEN 30

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob2$ gcc main_sv.c -o sv
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob2$ ./sv
Connection from (localhost, 53352)
Connection from (localhost, 53358)
Connection from (localhost, 35632)
Connection from (localhost, 35636)
Connection from (localhost, 35644)　# エファメラルポートのため番号が変わっている。TLPIの実行結果ではシーケンシャルだったが、この実行結果はシーケンシャルではなかった

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob2$ gcc main_cl.c -o cl
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob2$ ./cl localhost 1
Sequence number: 0
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob2$ ./cl localhost 1
Sequence number: 1
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob2$ ./cl localhost 3
Sequence number: 2
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob2$ ./cl localhost 800000
Sequence number: 5
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob2$ ./cl localhost 1
Sequence number: 800005

*/

// 例59-7を参考にする
int main(int argc, char *argv[]){
    int sfd=inetConnect(argv[1],PORT_NUM,SOCK_STREAM);

    char *reqLenStr=argv[2];
    if(write(sfd,reqLenStr,strlen(reqLenStr))!=strlen(reqLenStr)){
        perror("write");
        return 1;
    }
    if(write(sfd,"\n",1)!=1){
        perror("write");
        return 1;
    }

    int numRead=readLine(sfd,reqLenStr,INT_LEN);
    if(numRead<=0){
        perror("readLine");
        return 1;
    }

    printf("Sequence number: %s",reqLenStr);
    
    return 0;
}