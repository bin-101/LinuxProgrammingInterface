#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec13/prob4$ ./a.out 0
printf0
write0
write1printf1

出力が端末のとき、stdioバッファが行バッファになることを確認できた
*/
void test0(){
    printf("printf0\n");
    write(STDOUT_FILENO, "write0\n", 7);

    printf("printf1");
    write(STDOUT_FILENO, "write1", 6);
}

/*
// test.txt
writeprintf

printfをした後、ファイルディスクリプタを変更すると出力先が変わる。

*/
void test1(){
    printf("printf");
    int fd=open("test.txt",O_WRONLY|O_CREAT|O_TRUNC, 0777);
    dup2(fd,STDOUT_FILENO);
    write(STDOUT_FILENO, "write", 5);
}

/*
// test.txt
writeprintf
printf

出力先がファイルのとき、行バッファリングではないことを確認できた
*/
void test2(){
    int fd=open("test.txt",O_WRONLY|O_CREAT|O_TRUNC, 0777);
    dup2(fd,STDOUT_FILENO);
    printf("printf\n");
    write(STDOUT_FILENO, "write", 5);
    printf("printf");
}

int main(int argc, char *argv[]){
    if(argv[1][0]=='0'){
        test0();
    }else if(argv[1][0]=='1'){
        test1();
    }else if(argv[1][0]=='2'){
        test2();
    }
}