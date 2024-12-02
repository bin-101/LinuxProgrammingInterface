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
#include "../prob2/socket_library.h"

#define PORT_NUM "50000"
#define BACKLOG 50
#define MAXDATA 1000
#define ADDRSTRLEN (NI_MAXHOST+NI_MAXSERV+10)
#define REQUEST_LEN (5+30+10)

// 例60-4を参考にする

/*
実行結果
サーバー
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob4$ ./sv
Connection from (localhost, 54396)
command: get, id: 6, num: -1 from: (localhost, 54396)
no data
command: set, id: 6, num: 8 from: (localhost, 54396)
success set: id: 6, num: 8
Connection from (localhost, 54402)
command: delete, id: 6, num: -1 from: (localhost, 54402)
permission denied
command: delete, id: 6, num: -1 from: (localhost, 54396)
success delete: id: 6, num: 8
readLine: Success
Connection from (localhost, 54396) closed
readLine: Success
Connection from (localhost, 54402) closed
^C

クライアント1 ポート番号: 54396
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob4$ ./cl localhost
get 6
no data
set 6 8
success set: id: 6, num: 8
delete 6
success delete: id: 6, num: 8
(^Dで終了)

クライアント2 ポート番号: 54402
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec59/prob4$ ./cl localhost
delete 6
permission denied
(^Dで終了)
*/

struct Data{
    int num;
    char addrStr[ADDRSTRLEN]; // 書き込んだ人。一致する場合、データの削除と値の変更ができる
};
struct Data data[MAXDATA];

struct threadInfo{
    int cfd;
    char addrStr[ADDRSTRLEN];
};

pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handleRequest(void *void_info){
    struct threadInfo *info=(struct threadInfo *) void_info;
    char *addrStr=info->addrStr;
    printf("Connection from %s\n",addrStr);
    int cfd=info->cfd;

    while(TRUE){
        char reqLenStr[REQUEST_LEN]; // command id data (idとdataは両方数字) を受け取る
        if(readLine(info->cfd,reqLenStr,REQUEST_LEN)<=0){
            perror("readLine");
            printf("Connection from %s closed\n",addrStr);
            close(cfd);
            free(info);
            return NULL;
        }

        char command[10]; // set, get, delete
        int id,num=-1;
        char *str=strtok(reqLenStr, " "); // strtok: https://www.gavo.t.u-tokyo.ac.jp/~dsk_saito/lecture/software2/misc/misc03.html
        strcpy(command,str);
        str=strtok(NULL, " ");
        id=atoi(str);
        str=strtok(NULL, " ");
        if(str!=NULL){
            num=atoi(str);
        }
        printf("command: %s, id: %d, num: %d from: %s\n",command,id,num, addrStr);

        char res[100];

        pthread_mutex_lock(&data_mutex);

        if(id<0 || id>=MAXDATA){
            sprintf(res,"id out of range\n");
        }else if(strcmp(command,"get")==0){
            if(data[id].num==-1){
                sprintf(res,"no data\n");
            }else{
                sprintf(res,"success get: id: %d, num: %d\n",id,data[id].num);
            }
        }else if(strcmp(command,"delete")==0){
            if(data[id].num==-1){
                sprintf(res,"no data\n");
            }else if(strlen(data[id].addrStr)>0 && strcmp(data[id].addrStr,addrStr)!=0){
                sprintf(res,"permission denied\n");
            }else{
                sprintf(res,"success delete: id: %d, num: %d\n",id,data[id].num);
                data[id].num=-1;
                data[id].addrStr[0]='\0';
            }
        }else if(strcmp(command,"set")==0){
            if(strlen(data[id].addrStr)>0 && strcmp(data[id].addrStr,addrStr)!=0){
                sprintf(res,"permission denied\n");
            }else{
                sprintf(res,"success set: id: %d, num: %d\n",id,num);
                data[id].num=num;
                strcpy(data[id].addrStr,addrStr);
            }
        }else{
            sprintf(res,"command is invalid\n");
        }

        pthread_mutex_unlock(&data_mutex);

        printf("%s",res);
        write(cfd,res,strlen(res));
    }
    close(cfd);
    free(info);
    return NULL;
}

int main() {
    for(int i=0;i<MAXDATA;i++){
        data[i].num=-1;
        data[i].addrStr[0]='\0';
    }

    socklen_t socklen;
    int fd=inetListen(PORT_NUM,BACKLOG,&socklen);
    if(fd==-1){
        return 1;
    }

    while(TRUE){
        socklen_t addrlen=sizeof(struct sockaddr_storage);
        struct sockaddr_storage perr_addr;
        struct threadInfo *info=malloc(sizeof(struct threadInfo));
        info->cfd=accept(fd, (struct sockaddr *)&perr_addr, &addrlen);
        if(info->cfd==-1){
            perror("accept");
            return 1;
        }
        
        inetAddressStr((struct sockaddr *)&perr_addr,addrlen,info->addrStr,ADDRSTRLEN);

        pthread_t t;
        int s=pthread_create(&t,NULL,handleRequest,info);
        if(s!=0){
            perror("pthread_create");
            return 1;
        }
        pthread_detach(t);
    }
}