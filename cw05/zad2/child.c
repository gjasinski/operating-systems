#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF 50

double complex getRandomComplex();

int iters(double complex c, int K);

int main(int argc, char* argv[]) {
    if(argc != 4){
        printf("Wrong arguments - slave\n");
        return 0;
    }
    int N = atol(argv[2]);
    int K = atol(argv[3]);

    int fd = open(argv[1], O_WRONLY);
    if (fd<0) {
        printf("Can't open file - slave\n");
        exit(0);
    }
    srand(time(NULL));
    char buf[MAX_BUF];

    double complex c;

    for(int i = 0; i < N; i++){
        c = getRandomComplex();
        sprintf(buf, "%.19lf %.19lf %d ", creal(c), cimag(c), iters(c, K));
        write(fd, buf, MAX_BUF);
    }
    close(fd);
}

int iters(double complex c, int K){
    double complex z = 0.0;
    for(int i = 0; i < K; i++){
        z = cpow(z, 2.0);
        z += c;
        if(cabs(z) > 2.0){
            return i;
        }
    }
    return K;
}

double complex getRandomComplex(){
    return (-2 + (double)rand() / RAND_MAX * 3) + (-1 + (double)rand() / RAND_MAX * 2) * I;
}