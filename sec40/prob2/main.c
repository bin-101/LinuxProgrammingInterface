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
#include <sys/resource.h>
#include <utmpx.h>
#include <time.h>
#include <lastlog.h>

/*
例40-3を参考にした。

## 実行結果
- ./a.outとその他プログラムの実行は別ターミナルで行った

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec40/prob2$ sudo ./a.out bin101 30
Creating login entries int utmp and wetmp
         using pid 4360, line pts/2, id /2
Creating logout entries in utmp and wtmp
Last login: bin101 on Thu Jan  1 09:00:00 1970
 from

## 実行前
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/loginacct$ ./dump_utmpx
user     type            PID line   id  host     term exit session  address         date/time
reboot   BOOT_TIME (2)     0 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:30:55 2024
bin101   USER_PR   (7)  1029 tty2       tty2        0   0        0  0.0.0.0         Tue Nov 26 22:30:59 2024
runlevel RUN_LVL   (1)    53 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:31:20 2024
         DEAD_PR   (8)  4162 pts/2  ts/2             0   0        0  0.0.0.0         Tue Nov 26 22:33:43 2024
         DEAD_PR   (8)  4164 pts/2  /2              0   0        0  0.0.0.0         Tue Nov 26 22:33:43 2024
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/loginacct$ lastlog --user bin101
ユーザ名         ポート   場所             最近のログイン
bin101           pts/2    localhost        火 11月 26 22:33:43 +0900 2024

## sleepの間
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/loginacct$ ./dump_utmpx
user     type            PID line   id  host     term exit session  address         date/time
reboot   BOOT_TIME (2)     0 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:30:55 2024
bin101   USER_PR   (7)  1029 tty2       tty2        0   0        0  0.0.0.0         Tue Nov 26 22:30:59 2024
runlevel RUN_LVL   (1)    53 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:31:20 2024
bin101   USER_PR   (7)  4358 pts/2  ts/2b             0   0        0  0.0.0.0         Tue Nov 26 22:34:25 2024 # おそらくsudoが原因。下に詳細あり
bin101   USER_PR   (7)  4360 pts/2  /2              0   0        0  0.0.0.0         Tue Nov 26 22:34:25 2024 # プログラムが書き込んだもの

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/loginacct$ ./dump_utmpx /var/log/wtmp
~~
bin101   USER_PR   (7)  4360 pts/2  /2              0   0        0  0.0.0.0         Tue Nov 26 22:34:25 2024　# ログインが追加されていることを確認

## 実行後
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/loginacct$ ./dump_utmpx
user     type            PID line   id  host     term exit session  address         date/time
reboot   BOOT_TIME (2)     0 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:30:55 2024
bin101   USER_PR   (7)  1029 tty2       tty2        0   0        0  0.0.0.0         Tue Nov 26 22:30:59 2024
runlevel RUN_LVL   (1)    53 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:31:20 2024
         DEAD_PR   (8)  4358 pts/2  ts/2             0   0        0  0.0.0.0         Tue Nov 26 22:34:55 2024
         DEAD_PR   (8)  4360 pts/2  /2              0   0        0  0.0.0.0         Tue Nov 26 22:34:55 2024

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/loginacct$ ./dump_utmpx /var/log/wtmp
         DEAD_PR   (8)  4360 pts/2  /2              0   0        0  0.0.0.0         Tue Nov 26 22:34:55 2024 # ログアウトが追加されていることを確認

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/loginacct$ lastlog --user bin101
ユーザ名         ポート   場所             最近のログイン
bin101           pts/2    localhost        火 11月 26 22:34:55 +0900 2024 # 更新されていることを確認


## bin101   USER_PR   (7)  4358 pts/2  ts/2b が何によって書き込まれたのかを調べる
- おそらくsudoが原因

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec40/prob2$ sudo sleep 30
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/loginacct$ ./dump_utmpx
user     type            PID line   id  host     term exit session  address         date/time
reboot   BOOT_TIME (2)     0 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:30:55 2024
bin101   USER_PR   (7)  1029 tty2       tty2        0   0        0  0.0.0.0         Tue Nov 26 22:30:59 2024
runlevel RUN_LVL   (1)    53 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:31:20 2024
bin101   USER_PR   (7)  6033 pts/2  ts/2b             0   0        0  0.0.0.0         Tue Nov 26 22:51:44 2024
         DEAD_PR   (8)  5914 pts/2  /2              0   0        0  0.0.0.0         Tue Nov 26 22:48:35 2024


bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec40/prob2$ sleep 30
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/loginacct$ ./dump_utmpx
user     type            PID line   id  host     term exit session  address         date/time
reboot   BOOT_TIME (2)     0 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:30:55 2024
bin101   USER_PR   (7)  1029 tty2       tty2        0   0        0  0.0.0.0         Tue Nov 26 22:30:59 2024
runlevel RUN_LVL   (1)    53 ~      ~~  6.8.0-49-   0   0        0  0.0.0.0         Tue Nov 26 22:31:20 2024
         DEAD_PR   (8)  6033 pts/2  ts/2             2   0        0  0.0.0.0         Tue Nov 26 22:52:11 2024
         DEAD_PR   (8)  5914 pts/2  /2              0   0        0  0.0.0.0         Tue Nov 26 22:48:35 2024

下にさらに詳細
*/

int main(int argc, char *argv[]){
    printf("pid: %d\n", getpid());
    struct utmpx ut;
    memset(&ut, 0, sizeof(struct utmpx));
    ut.ut_type=USER_PROCESS;
    strncpy(ut.ut_user, argv[1], sizeof(ut.ut_user));
    time_t t;
    if(time(&t)==-1){
        printf("time: %s\n", strerror(errno));
        exit(1);
    }
    ut.ut_tv.tv_sec=t;
    ut.ut_pid=getpid();
    char *devName;
    devName = ttyname(STDIN_FILENO);
    if(devName==NULL || strlen(devName)<=8){
        printf("ttyname: %s\n", strerror(errno));
        exit(1);
    }
    strncpy(ut.ut_line, devName+5, sizeof(ut.ut_line));
    strncpy(ut.ut_id, devName+8, sizeof(ut.ut_id));
    printf("Creating login entries int utmp and wetmp\n");
    printf("         using pid %ld, line %s, id %s\n", (long)ut.ut_pid, ut.ut_line, ut.ut_id);

    setutxent();
    if(pututxline(&ut)==NULL){
        printf("pututxline: %s\n", strerror(errno));
        exit(1);
    }
    updwtmpx(_PATH_WTMP, &ut);

    sleep(atoi(argv[2]));
    
    ut.ut_type=DEAD_PROCESS;
    time(&t);
    ut.ut_tv.tv_sec=t;

    memset(&ut.ut_user, 0, sizeof(ut.ut_user));

    printf("Creating logout entries in utmp and wtmp\n");
    setutxent();
    if(pututxline(&ut)==NULL){
        printf("pututxline: %s\n", strerror(errno));
        exit(1);
    }
    updwtmpx(_PATH_WTMP, &ut);
    endutxent();

    struct lastlog llog;

    struct passwd *pwd;
    pwd=getpwnam(argv[1]);
    if(pwd==NULL){
        printf("getpwnam: %s\n", strerror(errno));
        exit(1);
    }
    uid_t uid=pwd->pw_uid;

    int fd=open(_PATH_LASTLOG, O_RDWR);
    if(lseek(fd, (uid-1)*sizeof(struct lastlog), SEEK_SET)==-1){ // 例40-4ではuid*sizeof(struct lastlog)だが、こっちのほうが正しく動作した
        printf("lseek: %s\n", strerror(errno));
        exit(1);
    }
    if(read(fd, &llog, sizeof(struct lastlog))==-1){
        printf("read: %s\n", strerror(errno));
        exit(1);
    }
    t=llog.ll_time;
    printf("Last login: %s on %s from %s\n", argv[1], ctime(&t), llog.ll_host);

    time(&t);
    llog.ll_time=t;
    strncpy(llog.ll_line, devName+5, sizeof(llog.ll_line));
    strncpy(llog.ll_host, "localhost", sizeof(llog.ll_host));

    write(fd, &llog, sizeof(struct lastlog));

    close(fd);
} 

/*
pty を割り当てた時に utmp/utmpx ファイルに記録を加えます と書いてあるからこれか？

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec40/prob2$ sudo sudo -V
Sudo バージョン 1.9.9
configure オプション: --build=x86_64-linux-gnu --prefix=/usr --includedir=${prefix}/include --mandir=${prefix}/share/man --infodir=${prefix}/share/info --sysconfdir=/etc --localstatedir=/var --disable-option-checking --disable-silent-rules --libdir=${prefix}/lib/x86_64-linux-gnu --runstatedir=/run --disable-maintainer-mode --disable-dependency-tracking -v --with-all-insults --with-pam --with-pam-login --with-fqdn --with-logging=syslog --with-logfac=authpriv --with-env-editor --with-editor=/usr/bin/editor --with-exampledir=/usr/share/doc/sudo/examples --with-timeout=15 --with-password-timeout=0 --with-passprompt=[sudo] password for %p:  --disable-root-mailer --with-sendmail=/usr/sbin/sendmail --with-rundir=/run/sudo --with-sssd --with-sssd-lib=/usr/lib/x86_64-linux-gnu --enable-zlib=system --with-selinux --with-linux-audit --enable-tmpfiles.d=yes --without-lecture --with-tty-tickets --enable-admin-flag
sudoers ポリシープラグイン  バージョン 1.9.9
sudoers ファイル文法バージョン 48

sudoers のパス: /etc/sudoers
認証方法: 'pam'
ログ記録時に syslog を使用する場合の syslog ファシリティ: authpriv
ログ記録時に syslog を使用する場合の syslog プライオリティ: notice
ユーザー認証に失敗したと時に使用される syslog プライオリティ: alert
ユーザー認証に失敗した場合にメールを送信します
ユーザー他 sudoers 内に存在しない場合にメールを送信します
ユーザーが最初に sudo を実行した時に講義を行う
デフォルトでユーザーが認証されていることを必要とします
root が sudo を実行するかもしれません
役に立つエラーメッセージを表示するためにいくつかの情報を収集することを許可します
sudoers ファイルに完全修飾ホスト名 (FQDN) を要求します
visudo が EDITOR 環境変数を尊重して使用します
LOGNAME および USER 環境変数を設定します
ログファイルの行頭から改行までの長さ (0 の場合は改行しない): 80
認証タイムスタンプのタイムアウト値: 15.0 分
パスワード入力要求のタイムアウト値: 0.0 分
パスワード入力の試行回数: 3
使用する umask 値 (0777 の場合はユーザーの設定値を使用します): 022
メールプログラムのパス: /usr/sbin/sendmail
メールプログラムの引数フラグ: -t
メールの送信先アドレス: root
メールの件名 (Subject) 行: *** SECURITY information for %h ***
パスワードを間違った時のメッセージ: すみません、もう一度試してください。
受講状況ディレクトリのパス: /var/lib/sudo/lectured
認証タイムスタンプディレクトリのパス: /run/sudo/ts
パスワード入力要求時に表示される文字列: [sudo] %p のパスワード: 
コマンドを実行するデフォルトの変更先ユーザー: root
ユーザーの $PATH を上書きする時の値: /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/snap/bin
visudo で使用されるエディターのパス: /usr/bin/editor
'list' 疑似コマンド使用するためにパスワードを要求される時: any
'verify' 疑似コマンドを使用するためにパスワードを要求される時: all
3 以上の値をもつファイル記述子をコマンド実行前に閉じます
環境変数の集合をデフォルトに設定します
安全性の確認を行う環境変数:
        TZ
        TERM
        LINGUAS
        LC_*
        LANGUAGE
        LANG
        COLORTERM
削除する環境変数:
        *=()*
        RUBYOPT
        RUBYLIB
        PYTHONUSERBASE
        PYTHONINSPECT
        PYTHONPATH
        PYTHONHOME
        TMPPREFIX
        ZDOTDIR
        READNULLCMD
        NULLCMD
        FPATH
        PERL5DB
        PERL5OPT
        PERL5LIB
        PERLLIB
        PERLIO_DEBUG
        JAVA_TOOL_OPTIONS
        SHELLOPTS
        BASHOPTS
        GLOBIGNORE
        PS4
        BASH_ENV
        ENV
        TERMCAP
        TERMPATH
        TERMINFO_DIRS
        TERMINFO
        _RLD*
        LD_*
        PATH_LOCALE
        NLSPATH
        HOSTALIASES
        RES_OPTIONS
        LOCALDOMAIN
        CDPATH
        IFS
保護する環境変数:
        XDG_CURRENT_DESKTOP
        XAUTHORIZATION
        XAUTHORITY
        PS2
        PS1
        PATH
        LS_COLORS
        KRB5CCNAME
        HOSTNAME
        DPKG_COLORS
        DISPLAY
        COLORS
sudoers を構文解析する時に使用するロケール: C
I/O ログを zlib を使用して圧縮します
常に疑似 tty 内でコマンドを実行します
入出力 (I/O) ログを保存するディレクトリです:/var/log/sudo-io
入出力 (I/O) ログを保存するファイルです:%{seq}
pty を割り当てた時に utmp/utmpx ファイルに記録を加えます
利用する PAM サービス名: sudo
ログインシェルで利用する PAM サービス名: sudo-i
ターゲットユーザーの PAM 資格情報による認証を試みる
実行するコマンドのために新しい PAM セッションを生成する
PAM アカウント検証管理を実行しています
sudoers のネットグループサポートを有効にする
ファイルを sudoedit で編集するときに親ディレクトリが書き込み可能か確かめる
監査ログファイルへの書き込みができなくても、コマンドの実行を許可する
ログファイルへの書き込みができなくても、コマンドの実行を許可する
ログエントリーがこの値より長くなると、複数の syslog メッセージに分割されます: 960
I/O ログのファイルモード: 0600
コマンドの実行時にパスでなくファイル記述子を使う: digest_only
認証タイムスタンプのタイプ: tty
ユーザー名の検索で大文字小文字を同一視する
グループ名の検索で大文字小文字を同一視する
コマンドが sudoers で許可された場合にログに記録します
コマンドが sudoers で拒否された場合にログに記録します
Sudo ログサーバーのタイムアウト、単位は秒: 30
ログサーバーに接続したソケットで SO_KEEPALIVE ソケットオプションを有効にする
ログサーバーの証明書が有効か検証する
PAMのリモートユーザーを sudo を実行しているユーザーに設定
生成するログの書式: sudo
SELinux RBAC のサポートを有効にする
sudo が最初に実行された時に作成されるファイルのパス: ~/.sudo_as_admin_successful
コアダンプファイルの最大サイズ(バイト単位): 0,0

ローカル IP アドレスとネットマスクの組:
        192.168.40.165/255.255.255.0
        2405:1200:838d:8200::ce45:1221/ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff
        2405:1200:838d:8200:646e:c1b0:fbcb:5754/ffff:ffff:ffff:ffff::
        2405:1200:838d:8200:bc3d:ad7a:445f:ff35/ffff:ffff:ffff:ffff::
        fe80::7f5d:b8b0:ee9b:c164/ffff:ffff:ffff:ffff::

Sudoers I/O plugin version 1.9.9
Sudoers audit plugin version 1.9.9
*/