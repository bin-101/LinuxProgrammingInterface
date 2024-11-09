#define _GNU_SOURCE
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/*
int initgroups(const char *user, gid_t group) を実装する
    setgroups()でグループリストを設定する

rootで実行
root@bin101-Inspiron-16-5635:/home/bin101/code/LinuxProgrammingInterface/sec09/prob3# ./a.out
0
0: root

1
4: adm
24: cdrom
27: sudo
30: dip
46: plugdev
122: lpadmin
134: lxd
135: sambashare
1001: outsider

1
4: adm
24: cdrom
27: sudo
27: sudo //2回出力された
30: dip
46: plugdev
122: lpadmin
134: lxd
135: sambashare
*/



int my_initgroups(const char *user, gid_t group)
{
    struct group *grp;
    gid_t gid_list[1000]={group};
    int ngroup=1;
    while((grp=getgrent())!=NULL){
        char **mem=grp->gr_mem;
        for(char **mem=grp->gr_mem;*mem!=NULL;mem++){
            if(strcmp(*mem,user)==0){
                gid_list[ngroup]=grp->gr_gid;
                ngroup++;
            }
        }
    }

    int res=setgroups(ngroup,gid_list);
    if(res==-1){
        printf("%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    endgrent();
    return 0;
}

void print_groups(){
    int ngroups=getgroups(0,NULL);
    gid_t groups[ngroups];
    int res=getgroups(ngroups,groups);
    if(res==-1){
        printf("%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<res;i++){
        struct group *grp=getgrgid(groups[i]);
        if(grp==NULL){
            printf("%s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("%d: %s\n",grp->gr_gid,grp->gr_name);
    }
}

int main()
{
    printf("0\n");
    print_groups();

    my_initgroups("bin101",1001); //1001: outsiderグループ bin101は所属していない 

    printf("\n1\n");
    print_groups();

    my_initgroups("bin101",27); //27: sudoグループ bin101は所属している

    printf("\n1\n");
    print_groups();
}
