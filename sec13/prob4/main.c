#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*
リダイレクトやprintfの改行を外してみると色々起こり謎

*/

void output(){
    printf("printf\n");
    write(STDOUT_FILENO, "write\n", 6);
}

int main(){
    output();
    int fd=open("test.txt",O_WRONLY|O_CREAT|O_TRUNC, 0777);
    int res=dup2(fd,STDOUT_FILENO);
    printf("res=%d\n",res);
    output();
}