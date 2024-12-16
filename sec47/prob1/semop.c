#define _GNU_SOURCE
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
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <limits.h>
#include <stddef.h>



// ほぼ、例47-8の写経



#define MAX_SEMOPS 1000

static int
parseOps(char *arg,struct sembuf sops[]){
    char *remaining;
    int numOps=0;
    for(remaining=arg;;numOps++){
        if(numOps>=MAX_SEMOPS){
            printf("too many\n");
            exit(1);
        }

        if(*remaining=='\0'){
            printf("trailing comma or empty argument: %s",arg);
            exit(1);
        }
        if(!isdigit((unsigned char )*remaining)){
            printf("digit: %s\n",arg);
            exit(1);
        }

        char *sign;
        sops[numOps].sem_num=strtol(remaining,&sign,10);
        if(*sign=='\0' || strchr("+-=",*sign)==NULL){
            printf("+-= %s\n",arg);
            exit(1);
        }
        if(!isdigit((unsigned char)*(sign+1))){
            printf("isdigit: %c (%s)", *sign,arg);
            exit(1);
        }

        char *flags;
        sops[numOps].sem_op=strtol(sign+1,&flags,10);
        if(*sign=='-'){
            sops[numOps].sem_op=-sops[numOps].sem_op;
        }else if(*sign=='='){
            if(sops[numOps].sem_op!=0){
                printf("=0\n");
                exit(1);
            }
        }

        sops[numOps].sem_flg=0;
        for(;;flags++){
            if(*flags=='n'){
                sops[numOps].sem_flg|=IPC_NOWAIT;
            }else if(*flags=='u'){
                sops[numOps].sem_flg|=SEM_UNDO;
            }else{
                break;
            }
        }

        if(*flags!=',' && *flags!='\0'){
            printf("end\n");
            exit(1);
        }

        char *comma=strchr(remaining,',');
        if(comma==NULL){
            break;
        }else{
            remaining=comma+1;
        }
    }
    return numOps+1;
}

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main(int argc,char *argv[]){

    if(argc==1){
        int semid=semget(IPC_PRIVATE,5,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
        printf("semid: %d\n",semid);
        union semun arg;
        arg.val=0;
        if(semctl(semid,0,SETVAL,arg)==-1){
            perror("semvtl");
            exit(1);
        }
        exit(0);
    }

    for(int ind=2;argv[ind]!=NULL;ind++){
        struct sembuf sops[MAX_SEMOPS];
        int nsops=parseOps(argv[ind],sops);
        time_t t=time(NULL);
        printf("%5ld, %s: about to semop() [%s]\n",(long)getpid(),ctime(&t),argv[ind]);

        if(semop(atoi(argv[1]),sops,nsops)==-1){
            perror("semop");
            exit(1);
        }

        printf("%5ld, %s: semop() completed [%s]\n",(long)getpid(),ctime(&t),argv[ind]);
    }
}

/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob1$ ./a.out
semid: 4
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob1$ ipcs -s -i 4

セマフォ配列 semid=4
uid=1000         gid=1000        cuid=1000       cgid=1000
モード=0600, アクセスパーミッション=0600
nsems = 5
otime = 未セット # semop成功時やSEM_UNDOの結果として変化
ctime = Mon Dec 16 20:49:53 2024  # 作成時や代入時に変化
semnum     値        ncount     zcount     pid       
0          0          0          0          63594
1          0          0          0          0         
2          0          0          0          0         
3          0          0          0          0         
4          0          0          0          0

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob1$ ./a.out 4 0-1n
63708, Mon Dec 16 20:50:28 2024
: about to semop() [0-1n]
semop: Resource temporarily unavailable # IPC_NOWAITフラグの効果

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob1$ ./a.out 4 0+3u
63816, Mon Dec 16 20:51:16 2024
: about to semop() [0+3u]
63816, Mon Dec 16 20:51:16 2024
: semop() completed [0+3u]
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob1$ ipcs -s -i 4

セマフォ配列 semid=4
uid=1000         gid=1000        cuid=1000       cgid=1000
モード=0600, アクセスパーミッション=0600
nsems = 5
otime = Mon Dec 16 20:51:16 2024  
ctime = Mon Dec 16 20:49:53 2024  
semnum     値        ncount     zcount     pid       
0          0          0          0          63816 # SEM_UNDOフラグをつけたため変化なし(pidは変化している)
1          0          0          0          0         
2          0          0          0          0         
3          0          0          0          0         
4          0          0          0          0

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob1$ ./a.out 4 0-1 &
[5] 63960
63960, Mon Dec 16 20:55:22 2024
: about to semop() [0-1]
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob1$ ./a.out 4 0+3u &
[6] 63968
63968, Mon Dec 16 20:55:29 2024
: about to semop() [0+3u]
63968, Mon Dec 16 20:55:29 2024
: semop() completed [0+3u]
63960, Mon Dec 16 20:55:22 2024
: semop() completed [0-1]
[5]-  終了                  ./a.out 4 0-1
[6]+  終了                  ./a.out 4 0+3u # -3できないが終了している
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob1$ ipcs -s -i 4

セマフォ配列 semid=4
uid=1000         gid=1000        cuid=1000       cgid=1000
モード=0600, アクセスパーミッション=0600
nsems = 5
otime = Mon Dec 16 20:55:29 2024  
ctime = Mon Dec 16 20:49:53 2024  
semnum     値        ncount     zcount     pid       
0          0          0          0          63968 # undoで0まで引いたのがわかる
1          0          0          0          0         
2          0          0          0          0         
3          0          0          0          0         
4          0          0          0          0
*/