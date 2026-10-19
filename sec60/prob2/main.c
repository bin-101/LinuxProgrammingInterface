#include "../prob1/become_daemon.h"
#include "../../sec59/prob2/socket_library.h"

// 例60-4を参考にした

/*
端末1
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec60/prob2$ gcc main.c
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec60/prob2$ systemd-socket-activate --listen=127.0.0.1:50000 --accept --inetd ./a.out -i

端末2
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface$ telnet 127.0.0.1 50000
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.
echo
echo

*/


#define SERVICE "echo"
#define BUF_SIZE 4096
#define MAX_CHILD_NUM 2


void grimReaper(int sig){
    int savedErrno=errno;
    while(waitpid(-1,NULL,WNOHANG)>0){
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
    // ここを追加しただけ
    if (argc > 1 && strcmp(argv[1], "-i") == 0) {
        handleRequest(STDIN_FILENO);
        exit(EXIT_SUCCESS);
    }
    
    if(becomeDaemon(0)==-1){
        printf("becomeDaemon: %s\n",strerror(errno));
    }

    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    sa.sa_handler=grimReaper;

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
                close(cfd);
                break;
        }
    }
}
