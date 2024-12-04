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
#include <syslog.h>

/*
下に調べたことを書いた
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec37/prob1$ ./a.out %s I
TLPI[12581]: %s

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec37/prob1$ tail -1 /var/log/syslog
Dec  4 22:21:03 bin101-Inspiron-16-5635 TLPI[12581]: %s
*/

int main(int argc,char *argv[]){
    int level;
    switch(argv[2][0]){
        case 'E':
            level = LOG_EMERG;
            break;
        case 'A':
            level = LOG_ALERT;
            break;
        case 'C':
            level = LOG_CRIT;
            break;
        case 'R': // Eが重複するため
            level = LOG_ERR;
            break;
        case 'W':
            level = LOG_WARNING;
            break;
        case 'N':
            level = LOG_NOTICE;
            break;
        case 'I':
            level = LOG_INFO;
            break;
        case 'D':
            level = LOG_DEBUG;
            break;
        default:
            fprintf(stderr,"Bad facility: %c\n",argv[2][0]);
            exit(EXIT_FAILURE);
    }

    const char *ident = "TLPI";
    openlog("TLPI",LOG_PID|LOG_PERROR,LOG_USER);
    syslog(level,"%s",argv[1]); // フォーマット文字列攻撃の対策
}

/*
/etc/syslog.confはなかったが、/etc/rsyslog.confはあった

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec37/prob1$ cat /etc/rsyslog.conf
~~~~
#
# Include all config files in /etc/rsyslog.d/
#
$IncludeConfig /etc/rsyslog.d/*.conf # ディレクトリの中のすべての.confファイルを読み込む

bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec37/prob1$ cat /etc/rsyslog.d/20-ufw.conf 
# Log kernel generated UFW log messages to file
:msg,contains,"[UFW " /var/log/ufw.log

# Uncomment the following to stop logging anything that matches the last rule.
# Doing this will stop logging kernel generated UFW log messages to the file
# normally containing kern.* messages (eg, /var/log/kern.log)
#& stop
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec37/prob1$ cat /etc/rsyslog.d/50-default.conf 
#  Default rules for rsyslog.
#
#                       For more information see rsyslog.conf(5) and /etc/rsyslog.conf

#
# First some standard log files.  Log by facility.
#
auth,authpriv.*                 /var/log/auth.log
*.*;auth,authpriv.none          -/var/log/syslog # facilityがauthとauthpriv以外のすべてのログを/var/log/syslogに書き込む
#cron.*                         /var/log/cron.log
#daemon.*                       -/var/log/daemon.log
kern.*                          -/var/log/kern.log
#lpr.*                          -/var/log/lpr.log
mail.*                          -/var/log/mail.log
#user.*                         -/var/log/user.log

#
# Logging for the mail system.  Split it up so that
# it is easy to write scripts to parse these files.
#
#mail.info                      -/var/log/mail.info
#mail.warn                      -/var/log/mail.warn
mail.err                        /var/log/mail.err

#
# Some "catch-all" log files.
#
#*.=debug;\
#       auth,authpriv.none;\
#       news.none;mail.none     -/var/log/debug
#*.=info;*.=notice;*.=warn;\
#       auth,authpriv.none;\
#       cron,daemon.none;\
#       mail,news.none          -/var/log/messages

#
# Emergencies are sent to everybody logged in.
#
*.emerg                         :omusrmsg:*

#
# I like to have messages displayed on the console, but only on a virtual
# console I usually leave idle.
#
#daemon,mail.*;\
#       news.=crit;news.=err;news.=notice;\
#       *.=debug;*.=info;\
#       *.=notice;*.=warn       /dev/tty8
*/
