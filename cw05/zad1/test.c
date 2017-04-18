#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main() {
    char* in = (char*)calloc(1000, sizeof(char));
    fgets(in, 1000, stdin);
    printf("%s test\n", in);
    return 0;
}