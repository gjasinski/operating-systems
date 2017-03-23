#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
    if(argc != 2){
        printf("%s\n", "Usage: demo <sleep time in s>");
        return -1;
    }
    sleep(strtol(argv[1], NULL, 10));
    return 0;
}
