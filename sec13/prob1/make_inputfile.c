#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
    int length=1000000;
    for(int i=0;i<length;i++){
        char x='a'+rand()%26;
        printf("%c",x);
    }
    printf("\n");
}