#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    printf("I'll eat your memory \n");
    char* a = (char*)malloc(1024*1024*100);
    for(int i = 0; i< 1024*1024*10; i++){
        a[i]=10;
    }
    return 0;
}
