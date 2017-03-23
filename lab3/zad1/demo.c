#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
    if(argc != 2){
        printf("%s\n", "Usage: demo <environment variable>");
        return -1;
    }
    char* env_var = getenv(argv[1]);
    if(!env_var) printf("%s - doesn't exists\n", argv[1]);
    else printf("%s %s\n", argv[1], env_var);
    return 0;
}
