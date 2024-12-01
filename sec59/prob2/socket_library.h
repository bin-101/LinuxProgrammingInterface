#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

typedef enum { FALSE, TRUE } Boolean;

/*
例59-9を参考にする
*/

/*
返り値
    - 成功: ファイルディスクリプタ
    - 失敗: -1
*/
int inetConnect(const char *host, const char *service, int type){
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family=AF_UNSPEC;
    hints.ai_canonname=NULL;
    hints.ai_addr=NULL;
    hints.ai_next=NULL;
    hints.ai_socktype=type;

    struct addrinfo *result;
    int s=getaddrinfo(host, service, &hints, &result);

    if(s!=0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    int sfd;
    struct addrinfo *rp;
    for(rp=result; rp!=NULL; rp=rp->ai_next){
        sfd=socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sfd==-1){
            continue;
        }

        if(connect(sfd,rp->ai_addr,rp->ai_addrlen)!=-1){
            break;
        }
        close(sfd);
    }

    freeaddrinfo(result);

    return (rp==NULL)?-1:sfd;
}

static int inetPassiveSocket(const char *service, int type, socklen_t *addrlen, Boolean doListen, int backlog){
    struct addrinfo hints;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_canonname=NULL;
    hints.ai_addr=NULL;
    hints.ai_next=NULL;
    hints.ai_socktype=type;
    hints.ai_family=AF_UNSPEC;
    hints.ai_flags=AI_PASSIVE;

    struct addrinfo *result;
    int s=getaddrinfo(NULL, service, &hints, &result);
    if(s!=0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    int optval=1;
    int sfd;
    struct addrinfo *rp;
    for(rp=result; rp!=NULL; rp=rp->ai_next){
        sfd=socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sfd==-1){
            continue;
        }

        if(doListen){
            if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval))==-1){
                close(sfd);
                freeaddrinfo(result);
                return -1;
            }
        }
        if(bind(sfd,rp->ai_addr,rp->ai_addrlen)==0){
            break;
        }
        close(sfd);
    }

    if(rp!=NULL && doListen){
        if(listen(sfd,backlog)==-1){
            freeaddrinfo(result);
            return -1;
        }
    }

    if(rp!=NULL && addrlen!=NULL){
        *addrlen=rp->ai_addrlen;
    }
    freeaddrinfo(result);

    return (rp==NULL)?-1:sfd;
}

int inetListen(const char *service, int backlog, socklen_t *addrlen){
    return inetPassiveSocket(service, SOCK_STREAM, addrlen, TRUE, backlog);
}

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen){
    char host[NI_MAXHOST], service[NI_MAXSERV];

    if(getnameinfo(addr,addrlen,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICSERV)==0){
        snprintf(addrStr,addrStrLen,"(%s, %s)",host,service);
    }else{
        snprintf(addrStr,addrStrLen,"(?UNKNOWN?)");
    }
    return addrStr;
}

// 例59-1を参考にする
ssize_t readLine(int fd,void *buffer,size_t n){
    if(n<=0 || buffer==NULL){
        errno=EINVAL; // invalid argument
        return -1;
    }

    char *buf=buffer;
    size_t totRead=0;
    while(TRUE){
        char ch;
        ssize_t numRead=read(fd,&ch,1);
        if(numRead==-1){
            if(errno==EINTR){ // Interrupted system call
                continue;
            }else{
                return -1;
            }
        }else if(numRead==0){
            if(totRead==0){
                return 0;
            }else{
                break;
            }
        }else{
            if(totRead<n-1){
                totRead++;
                *buf++=ch;
            }
            if(ch=='\n'){
                break;
            }
        }
    }
    *buf='\0';
    return totRead;
}