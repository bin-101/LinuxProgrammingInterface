- 元の設定
    - `#define BUF_SIZE 1024`
    - `O_SYNC`なし
        - ファイルディスクリプタに対する書き込みを同期的にやる
    ```
    bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/fileio$ time ./copy /home/bin101/code/LinuxProgrammingInterface/sec13/prob1/inputfile.txt /home/bin101/code/LinuxProgrammingInterface/sec13/prob1/outputfile.txt

    real    0m0.010s
    user    0m0.003s
    sys     0m0.007s
    ```
- `O_SYNC`をつけた
    ```
    bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/fileio$ time ./copy /home/bin101/code/LinuxProgrammingInterface/sec13/prob1/inputfile.txt /home/bin101/codeLinuxProgrammingInterface/sec13/prob1/outputfile.txt

    real    0m1.088s
    user    0m0.004s
    sys     0m0.037s
    ```
- `#define BUF_SIZE 64`
    ```
    bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/fileio$ time ./copy /home/bin101/code/LinuxProgrammingInterface/sec13/prob1/inputfile.txt /home/bin101/code/LinuxProgrammingInterface/sec13/prob1/outputfile.txt

    real    0m16.856s
    user    0m0.016s
    sys     0m0.486s
    ```
    - 約16倍になった
- ファイルシステムを調べてみる
    ```
    bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/tlpi-240311-dist/tlpi-dist/fileio$ df -T
    Filesystem     Type     1K-blocks     Used Available Use% Mounted on
    tmpfs          tmpfs      1562624     2260   1560364   1% /run
    /dev/nvme0n1p7 ext4     439652800 13506740 403739456   4% /
    tmpfs          tmpfs      7813100   103124   7709976   2% /dev/shm
    tmpfs          tmpfs         5120        4      5116   1% /run/lock
    efivarfs       efivarfs       148      125        19  87% /sys/firmware/efi/efivars
    /dev/nvme0n1p1 vfat        200704    99454    101250  50% /boot/efi
    tmpfs          tmpfs      1562620     4740   1557880   1% /run/user/1000
    ```
    - ext4で実験したらしい