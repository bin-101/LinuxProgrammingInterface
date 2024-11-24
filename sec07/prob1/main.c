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


*/


//  free_and_sbrk.cを参考に実装

#define MAX_ALLOCS 1000000

int main(){
    char *ptr[MAX_ALLOCS];
    const int blockSize=10240;
    const int numAllocs=100;
    const int freeStep=1;
    const int freeMin=1;
    const int freeMax=numAllocs;

    printf("\n");
    printf("Initial program break: %10p\n", sbrk(0)); // sbrk(0)は現在のプログラムブレークを返す

    printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
    void *now_program_break=sbrk(0);
    for(int j=0;j<numAllocs;j++){
        ptr[j]=malloc(blockSize);
        if(ptr[j]==NULL){
            printf("malloc: %s", strerror(errno));
        }
        if(sbrk(0)!=now_program_break){
            printf("%d: now program break is: %10p\n", j, sbrk(0));
            now_program_break=sbrk(0);
        }
    }

    printf("Program break is now: %10p\n", sbrk(0));
    printf("Freeing blocks from %d to %d in steps of %d\n", freeMin, freeMax, freeStep);


    for(int j=freeMax-1;j>=freeMin-1;j-=freeStep){
        printf("%d: %10p\n", j, ptr[j]);
        free(ptr[j]);
        if(sbrk(0)!=now_program_break){
            printf("%d: now program break is: %10p\n", j, sbrk(0));
            now_program_break=sbrk(0);
        }
    }

    printf("After free(), program break is: %20p\n", sbrk(0));
}

/*
- 実験結果から分かったこと
    - malloc()によるメモリ確保
        - 16^3=40320バイト単位でプログラムブレークが変化している
    - free()によるメモリ開放
        - ある程度余裕をもたせてメモリを開放している。そのため、最初の3回はプログラムブレークが変化しない
実行結果
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec07/prob1$ ./a.out

Initial program break: 0x64a4dbc4b000
Allocating 100*10240 bytes
13: now program break is: 0x64a4dbc6e000
26: now program break is: 0x64a4dbc8f000
40: now program break is: 0x64a4dbcb2000
54: now program break is: 0x64a4dbcd5000
68: now program break is: 0x64a4dbcf8000
82: now program break is: 0x64a4dbd1b000
96: now program break is: 0x64a4dbd3e000
Program break is now: 0x64a4dbd3e000
Freeing blocks from 1 to 100 in steps of 1
99: 0x64a4dbd224e0
98: 0x64a4dbd1fcd0
97: 0x64a4dbd1d4c0
96: 0x64a4dbd1acb0
96: now program break is: 0x64a4dbd3b000
95: 0x64a4dbd184a0
95: now program break is: 0x64a4dbd39000
94: 0x64a4dbd15c90
94: now program break is: 0x64a4dbd36000
93: 0x64a4dbd13480
93: now program break is: 0x64a4dbd34000
92: 0x64a4dbd10c70
92: now program break is: 0x64a4dbd31000
91: 0x64a4dbd0e460
91: now program break is: 0x64a4dbd2f000
90: 0x64a4dbd0bc50
90: now program break is: 0x64a4dbd2c000
89: 0x64a4dbd09440
89: now program break is: 0x64a4dbd2a000
88: 0x64a4dbd06c30
88: now program break is: 0x64a4dbd27000
87: 0x64a4dbd04420
87: now program break is: 0x64a4dbd25000
86: 0x64a4dbd01c10
86: now program break is: 0x64a4dbd22000
85: 0x64a4dbcff400
85: now program break is: 0x64a4dbd20000
84: 0x64a4dbcfcbf0
84: now program break is: 0x64a4dbd1d000
83: 0x64a4dbcfa3e0
83: now program break is: 0x64a4dbd1b000
82: 0x64a4dbcf7bd0
82: now program break is: 0x64a4dbd18000
81: 0x64a4dbcf53c0
81: now program break is: 0x64a4dbd16000
80: 0x64a4dbcf2bb0
80: now program break is: 0x64a4dbd13000
79: 0x64a4dbcf03a0
79: now program break is: 0x64a4dbd11000
78: 0x64a4dbcedb90
78: now program break is: 0x64a4dbd0e000
77: 0x64a4dbceb380
77: now program break is: 0x64a4dbd0c000
76: 0x64a4dbce8b70
76: now program break is: 0x64a4dbd09000
75: 0x64a4dbce6360
75: now program break is: 0x64a4dbd07000
74: 0x64a4dbce3b50
74: now program break is: 0x64a4dbd04000
73: 0x64a4dbce1340
73: now program break is: 0x64a4dbd02000
72: 0x64a4dbcdeb30
72: now program break is: 0x64a4dbcff000
71: 0x64a4dbcdc320
71: now program break is: 0x64a4dbcfd000
70: 0x64a4dbcd9b10
70: now program break is: 0x64a4dbcfa000
69: 0x64a4dbcd7300
69: now program break is: 0x64a4dbcf8000
68: 0x64a4dbcd4af0
68: now program break is: 0x64a4dbcf5000
67: 0x64a4dbcd22e0
67: now program break is: 0x64a4dbcf3000
66: 0x64a4dbccfad0
66: now program break is: 0x64a4dbcf0000
65: 0x64a4dbccd2c0
65: now program break is: 0x64a4dbcee000
64: 0x64a4dbccaab0
64: now program break is: 0x64a4dbceb000
63: 0x64a4dbcc82a0
63: now program break is: 0x64a4dbce9000
62: 0x64a4dbcc5a90
62: now program break is: 0x64a4dbce6000
61: 0x64a4dbcc3280
61: now program break is: 0x64a4dbce4000
60: 0x64a4dbcc0a70
60: now program break is: 0x64a4dbce1000
59: 0x64a4dbcbe260
59: now program break is: 0x64a4dbcdf000
58: 0x64a4dbcbba50
58: now program break is: 0x64a4dbcdc000
57: 0x64a4dbcb9240
57: now program break is: 0x64a4dbcda000
56: 0x64a4dbcb6a30
56: now program break is: 0x64a4dbcd7000
55: 0x64a4dbcb4220
55: now program break is: 0x64a4dbcd5000
54: 0x64a4dbcb1a10
54: now program break is: 0x64a4dbcd2000
53: 0x64a4dbcaf200
53: now program break is: 0x64a4dbcd0000
52: 0x64a4dbcac9f0
52: now program break is: 0x64a4dbccd000
51: 0x64a4dbcaa1e0
51: now program break is: 0x64a4dbccb000
50: 0x64a4dbca79d0
50: now program break is: 0x64a4dbcc8000
49: 0x64a4dbca51c0
49: now program break is: 0x64a4dbcc6000
48: 0x64a4dbca29b0
48: now program break is: 0x64a4dbcc3000
47: 0x64a4dbca01a0
47: now program break is: 0x64a4dbcc1000
46: 0x64a4dbc9d990
46: now program break is: 0x64a4dbcbe000
45: 0x64a4dbc9b180
45: now program break is: 0x64a4dbcbc000
44: 0x64a4dbc98970
44: now program break is: 0x64a4dbcb9000
43: 0x64a4dbc96160
43: now program break is: 0x64a4dbcb7000
42: 0x64a4dbc93950
42: now program break is: 0x64a4dbcb4000
41: 0x64a4dbc91140
41: now program break is: 0x64a4dbcb2000
40: 0x64a4dbc8e930
40: now program break is: 0x64a4dbcaf000
39: 0x64a4dbc8c120
39: now program break is: 0x64a4dbcad000
38: 0x64a4dbc89910
38: now program break is: 0x64a4dbcaa000
37: 0x64a4dbc87100
37: now program break is: 0x64a4dbca8000
36: 0x64a4dbc848f0
36: now program break is: 0x64a4dbca5000
35: 0x64a4dbc820e0
35: now program break is: 0x64a4dbca3000
34: 0x64a4dbc7f8d0
34: now program break is: 0x64a4dbca0000
33: 0x64a4dbc7d0c0
33: now program break is: 0x64a4dbc9e000
32: 0x64a4dbc7a8b0
32: now program break is: 0x64a4dbc9b000
31: 0x64a4dbc780a0
31: now program break is: 0x64a4dbc99000
30: 0x64a4dbc75890
30: now program break is: 0x64a4dbc96000
29: 0x64a4dbc73080
29: now program break is: 0x64a4dbc94000
28: 0x64a4dbc70870
28: now program break is: 0x64a4dbc91000
27: 0x64a4dbc6e060
27: now program break is: 0x64a4dbc8f000
26: 0x64a4dbc6b850
26: now program break is: 0x64a4dbc8c000
25: 0x64a4dbc69040
25: now program break is: 0x64a4dbc8a000
24: 0x64a4dbc66830
24: now program break is: 0x64a4dbc87000
23: 0x64a4dbc64020
23: now program break is: 0x64a4dbc85000
22: 0x64a4dbc61810
22: now program break is: 0x64a4dbc82000
21: 0x64a4dbc5f000
21: now program break is: 0x64a4dbc80000
20: 0x64a4dbc5c7f0
20: now program break is: 0x64a4dbc7d000
19: 0x64a4dbc59fe0
19: now program break is: 0x64a4dbc7a000
18: 0x64a4dbc577d0
18: now program break is: 0x64a4dbc78000
17: 0x64a4dbc54fc0
17: now program break is: 0x64a4dbc75000
16: 0x64a4dbc527b0
16: now program break is: 0x64a4dbc73000
15: 0x64a4dbc4ffa0
15: now program break is: 0x64a4dbc70000
14: 0x64a4dbc4d790
14: now program break is: 0x64a4dbc6e000
13: 0x64a4dbc4af80
13: now program break is: 0x64a4dbc6b000
12: 0x64a4dbc48770
12: now program break is: 0x64a4dbc69000
11: 0x64a4dbc45f60
11: now program break is: 0x64a4dbc66000
10: 0x64a4dbc43750
10: now program break is: 0x64a4dbc64000
9: 0x64a4dbc40f40
9: now program break is: 0x64a4dbc61000
8: 0x64a4dbc3e730
8: now program break is: 0x64a4dbc5f000
7: 0x64a4dbc3bf20
7: now program break is: 0x64a4dbc5c000
6: 0x64a4dbc39710
6: now program break is: 0x64a4dbc5a000
5: 0x64a4dbc36f00
5: now program break is: 0x64a4dbc57000
4: 0x64a4dbc346f0
4: now program break is: 0x64a4dbc55000
3: 0x64a4dbc31ee0
3: now program break is: 0x64a4dbc52000
2: 0x64a4dbc2f6d0
2: now program break is: 0x64a4dbc50000
1: 0x64a4dbc2cec0
1: now program break is: 0x64a4dbc4d000
0: 0x64a4dbc2a6b0
0: now program break is: 0x64a4dbc4b000
After free(), program break is: 0x64a4dbc4b000
*/