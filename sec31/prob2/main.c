#define _GNU_SOURCE
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

/*
dirnameとbasenameの仕様
- 末尾のスラッシュを無視 done
- スラッシュ1つの場合は"/"を返す done
- NULLもしくは空文字列の場合は"."を返す done
- (おそらく)連続するスラッシュは1つにまとめる done
- (おそらく)連続する.は1つにまとめる done

dirnameの仕様
- スラッシュがなければ"." done

basenameの仕様
- スラッシュがなければそのまま done

実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec31/prob2$ ./a.out
/
. , .
/usr/bin/zip
/usr/bin , zip
/etc/passwd///
/etc , passwd
/etc///passwd/
/etc , passwd
etc/passwd
etc , passwd
passwd/
. , passwd
..
. , .
NULL
. , .

130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000833480256: aaa , aaaa
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000833480256: aaa , aaaa
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
130000822994496: bbb , bbb
*/

__thread char buf_dirname[PATH_MAX];
__thread char buf_basename[NAME_MAX];

// pathnameを変化させる
void func(char *pathname,char *temp, char *res){
    res[0]='\0';
    if(pathname == NULL || pathname[0] == '\0' || strcmp(pathname, "/") == 0){
        strcpy(res, ".");
        return;
    }
    strcpy(temp, pathname);

    //末尾のスラッシュを削除
    for(int i = strlen(temp) - 1; i >= 0; i--){
        if(temp[i] == '/'){
            if(i == 0){
                strcpy(res, "/");
                return;
            }
            temp[i]='\0';
        }else{
            break;
        }
    }

    // 連続するスラッシュと.を1つにまとめる
    char temp2[PATH_MAX];
    int idx2 = 0;
    for(int idx1 = 0; idx1 <= strlen(temp); idx1++){ //末尾の'\0'もtemp2idxに追加してほしいため<=にした
        if(temp[idx1] == '/'){
            if(idx2 == 0 || temp2[idx2 - 1] != '/'){
                temp2[idx2++] = temp[idx1];
            }
        }else if(temp[idx1] == '.'){
            if(idx2 ==0 || temp[idx2 -1] != '.'){
                temp2[idx2++] = temp[idx1];
            }
        }else{
            temp2[idx2++] = temp[idx1];
        }
    }
    res[idx2] = '\0';
    strcpy(temp, temp2);
}

int find_last_slash(char *pathname){
    for(int i = strlen(pathname) - 1; i >= 0; i--){
        if(pathname[i] == '/'){
            return i;
        }
    }
    return -1;
}

char *my_dirname(char *pathname){
    char temp[PATH_MAX];
    func(pathname, temp, buf_dirname);
    if(strlen(buf_dirname)){
        return buf_dirname;
    }

    int last_slash = find_last_slash(temp);
    if(last_slash == -1){
        return ".";
    }

    temp[last_slash] = '\0';
    strcpy(buf_dirname, temp);
    return buf_dirname;
}

char *my_basename(char *pathname){
    char temp[PATH_MAX];
    func(pathname, temp, buf_basename);
    if(strlen(buf_basename)){
        return buf_basename;
    }

    int last_slash = find_last_slash(temp);
    if(last_slash == -1){
        strcpy(buf_basename, temp);
        return buf_basename;
    }

    strcpy(buf_basename, temp + last_slash+1);
    return buf_basename;
}

void* threadFunc(void *arg){
    for(int i=0;i<30;i++){
        printf("%ld: %s , %s\n",pthread_self(),my_dirname((char*)arg),my_basename((char *)arg));
    }
    return NULL;
}

int main(){
    char *pathnames[] = {
        "/",
        "/usr/bin/zip",
        "/etc/passwd///",
        "/etc///passwd/",
        "etc/passwd",
        "passwd/",
        "..",
        NULL
    };
    for(int i=0;i<8;i++){
        if(pathnames[i]==NULL) printf("NULL\n");
        else printf("%s\n", pathnames[i]);
        printf("%s , %s\n",my_dirname(pathnames[i]),my_basename(pathnames[i]));
    }

    // スレッドセーフになっているか確認

    pthread_t t1,t2;
    char t1_str[]="aaa/aaaa";
    char t2_str[]="bbb/bbb";

    pthread_create(&t1,NULL,threadFunc,(void *)t1_str);
    pthread_create(&t2,NULL,threadFunc,(void *)t2_str);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
}