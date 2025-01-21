#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec04/prob1$ ./a.out output.txt <input.txt
TEE(1)                                                                                             User Commands                                                                                            TEE(1)

NAME
       tee - read from standard input and write to standard output and files
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec04/prob1$ gcc main.c
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec04/prob1$ ./a.out output_a.txt -a <input.txt
option a
TEE(1)                                                                                             User Commands                                                                                            TEE(1)

NAME
       tee - read from standard input and write to standard output and files
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec04/prob1$ ./a.out output_a.txt -a <input.txt
option a
TEE(1)                                                                                             User Commands                                                                                            TEE(1)

NAME
       tee - read from standard input and write to standard output and files
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec04/prob1$ ./a.out output.txt <input.txt
TEE(1)                                                                                             User Commands                                                                                            TEE(1)

NAME
       tee - read from standard input and write to standard output and files

output.txtは上書きされ、output_a.txtは追記されていることを確かめた
*/

int main(int argc,char *argv[]){

    int opt;
    int is_a=0;
    while((opt=getopt(argc,argv,"a"))!=-1){
        switch(opt){
            case 'a':
                printf("option a\n");
                is_a=1;
                break;
            default:
                perror("default");
                exit(1);
        }
    }
    char *file_path=argv[optind];
    int flag=0;
    if(is_a){
        flag=(O_WRONLY|O_CREAT|O_APPEND);
    }else{
        flag=(O_WRONLY|O_CREAT|O_TRUNC);
    }
    int fd=open(file_path,flag,S_IWUSR|S_IRUSR);

    const int size=10000;
    char buffer[size];
    ssize_t numRead=read(STDIN_FILENO,buffer,size);
    write(STDOUT_FILENO,buffer,numRead);
    write(fd,buffer,numRead);

    close(fd);
}