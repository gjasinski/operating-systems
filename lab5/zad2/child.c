#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if(argc != 4){
        printf("Wrong arguments");
        return 0;
    }
    int N = atol(argv[2]);
    int K = atol(argv[3]);
    //int status =
    mkfifo(argv[1], 0);
    //FILE* stream = fopen(argv[1], O_RDONLY);
    srand(time(NULL));
    //char* buf = (char*)calloc(100, sizeof(char));
    double complex c;
    for(int i = 0; i < N; i++){
        //strcpy(buf, )
        //fwrite("%f %f %d");
        c = rand();
        printf("%f i%f\n", creal(c), cimag(c));
        K=N=K;
    }
}

int iters(double complex c, int K){
    double complex z = 0.0;
    double complex pow_2 = 2.0;
    int res = 0;
    while(1){
        z = cpow(z, pow_2) + c;
        res++;
        if(cabs(z) <= 2.0){
            int res_prim = res;
            for(int i = 0; i < K; i++){
                z = cpow(z, 2.0) + c;
                res_prim++;
            }
            if(cabs(z) <= 2.0){
                return res;
            }
            else{
                res = res_prim;
            }
        }
    }
}