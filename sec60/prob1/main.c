#include "become_daemon.h"
#include "../../sec59/prob2/socket_library.h"

// 例60-4を参考にした

/*
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec60/prob1$ sudo ./a.out
[sudo] bin101 のパスワード:
端末1
bin101@bin101-Inspiron-16-5635:~$ telnet localhost echo
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
hello
hello
端末2
bin101@bin101-Inspiron-16-5635:~$ telnet localhost echo
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
hello2
hello2
端末3
bin101@bin101-Inspiron-16-5635:~$ telnet localhost echo
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
hello3 # サーバーから返ってこない

切断してみる
bin101@bin101-Inspiron-16-5635:~$ telnet localhost echo
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
hello
hello
^]
telnet> q
Connection closed.
端末3の出力が確認できた
bin101@bin101-Inspiron-16-5635:~$ telnet localhost echo
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
hello3
hello3
*/

#define SERVICE "echo"
#define BUF_SIZE 4096
#define MAX_CHILD_NUM 2

volatile int child_num=0; // 変数がレジスタに割り当てられるのを防ぐ

void grimReaper(int sig){
    int savedErrno=errno;
    while(waitpid(-1,NULL,WNOHANG)>0){
        child_num--;
        continue;
    }
    errno=savedErrno;
}

void handleRequest(int cfd){
    char buf[BUF_SIZE];
    ssize_t numRead;

    while((numRead=read(cfd,buf,BUF_SIZE))>0){
        if(write(cfd,buf,numRead)!=numRead){
            syslog(LOG_ERR,"write() failed: %s",strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    if(numRead==-1){
        syslog(LOG_ERR,"Error from read(): %s",strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int main(int argc,char *argv[]){
    if(becomeDaemon(0)==-1){
        printf("becomeDaemon: %s\n",strerror(errno));
    }

    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    sa.sa_handler=grimReaper;

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGCHLD);
    sigprocmask(SIG_BLOCK,&mask,NULL);

    if(sigaction(SIGCHLD,&sa,NULL)==-1){
        syslog(LOG_ERR,"sigaction failed: %s",strerror(errno));
        exit(EXIT_FAILURE);
    }

    int lfd=inetListen(SERVICE,10,NULL);  // backlg=10 接続要求の保留の最大数
    if(lfd==-1){
        syslog(LOG_ERR,"Could not create server socket (%s)",strerror(errno));
        exit(EXIT_FAILURE);
    }

    while(TRUE){
        while(child_num>=MAX_CHILD_NUM){
            sigset_t emptyMask;
            sigemptyset(&emptyMask); // waitpid()をずっと実行する方法もあるが、sigsuspend()を使うことで、シグナルを受け取るまでプロセスを停止させることができる(多分)
            sigsuspend(&emptyMask); // child_num++の計算時にシグナルを受け取ると、child_numの値がおかしくなるため、ここで必ずシグナルを受け取る
        }

        int cfd=accept(lfd,NULL,NULL);
        if(cfd==-1){
            syslog(LOG_ERR,"Failure in accept(): %s",strerror(errno));
            exit(EXIT_FAILURE);
        }

        switch(fork()){
            case -1:
                syslog(LOG_ERR,"Can't create child (%s)",strerror(errno));
                close(cfd);
                break;
            case 0:
                close(lfd);
                handleRequest(cfd);
                _exit(EXIT_SUCCESS);
            default:
                child_num++;
                close(cfd);
                break;
        }
    }
}