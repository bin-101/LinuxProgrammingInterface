#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*
現状の理解
・printfの改行を外すとファイルにprintfが出力される
→ファイルディスクリプタと出力先の対応はstdioバッファ→カーネルバッファキャッシュの途中。
・ファイルへの出力順が、
    ・printfの改行を外す: write→printfの順番
    ・printfに改行をつける: printf→writeの順番
→行バッファリングになっている。出力先がファイルでもファイルディスクリプタが1なら行バッファになる？
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