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


/*
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ ipcs -s -i 7

セマフォ配列 semid=7
uid=1000         gid=1000        cuid=1000       cgid=1000
モード=0600, アクセスパーミッション=0600
nsems = 5
otime = 未セット              
ctime = Mon Dec 16 22:10:46 2024  
semnum     値        ncount     zcount     pid       
0          0          0          0          0         
1          0          0          0          0         
2          0          0          0          0         
3          0          0          0          0         
4          0          0          0          0

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ ./a.out 7 0-1 &
[1] 65859
65859, Mon Dec 16 22:11:39 2024
: about to semop() [0-1]

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ ./a.out 7 0+1 &
[2] 65877
65877, Mon Dec 16 22:11:45 2024
: about to semop() [0+1]
65877, Mon Dec 16 22:11:45 2024
: semop() completed [0+1]
65859, Mon Dec 16 22:11:39 2024
: semop() completed [0-1]
[1]-  終了                  ./a.out 7 0-1
[2]+  終了                  ./a.out 7 0+1

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ ipcs -s -i 7

セマフォ配列 semid=7
uid=1000         gid=1000        cuid=1000       cgid=1000
モード=0600, アクセスパーミッション=0600
nsems = 5
otime = Mon Dec 16 22:11:45 2024  
ctime = Mon Dec 16 22:10:46 2024  
semnum     値        ncount     zcount     pid       
0          0          0          0          65859  # -1をしたプロセスのID
1          0          0          0          0         
2          0          0          0          0         
3          0          0          0          0         
4          0          0          0          0

sleep(10)とprintf("end\n")を最後に実行

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ ./a.out 14 0+1u &
[1] 66787
66787, Mon Dec 16 22:18:56 2024
: about to semop() [0+1u]
66787, Mon Dec 16 22:18:56 2024
: semop() completed [0+1u]

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ ./a.out 14 0+2 &
[2] 66793
66793, Mon Dec 16 22:18:58 2024
: about to semop() [0+2]
66793, Mon Dec 16 22:18:58 2024
: semop() completed [0+2]

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ ipcs -s -i 14

セマフォ配列 semid=14
uid=1000         gid=1000        cuid=1000       cgid=1000
モード=0600, アクセスパーミッション=0600
nsems = 5
otime = Mon Dec 16 22:18:58 2024  
ctime = Mon Dec 16 22:18:47 2024  
semnum     値        ncount     zcount     pid       
0          3          0          0          66793 # 0+2を実行したプロセスのプロセスID
1          0          0          0          0         
2          0          0          0          0         
3          0          0          0          0         
4          0          0          0          0         

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ end

[1]-  終了                  ./a.out 14 0+1u

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ end

[2]+  終了                  ./a.out 14 0+2

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec47/prob3$ ipcs -s -i 14

セマフォ配列 semid=14
uid=1000         gid=1000        cuid=1000       cgid=1000
モード=0600, アクセスパーミッション=0600
nsems = 5
otime = Mon Dec 16 22:19:06 2024  
ctime = Mon Dec 16 22:18:47 2024  
semnum     値        ncount     zcount     pid       
0          2          0          0          66787 # 0+1uを実行したプロセスのプロセスID
1          0          0          0          0         
2          0          0          0          0         
3          0          0          0          0         
4          0          0          0          0         
*/

// 47-1からコピペ
// 47-1は不要な初期化を行っていたため削除


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

int main(int argc,char *argv[]){

    if(argc==1){
        int semid=semget(IPC_PRIVATE,5,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
        printf("semid: %d\n",semid);
        printf("pid: %d\n",getpid());
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
    sleep(10);
    printf("end\n");
}
