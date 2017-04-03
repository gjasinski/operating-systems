#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>

double real_time;

void handle_sigusr2(int i);

int main() {
    signal(SIGUSR2, &handle_sigusr2);
    srand(time(NULL));
    sleep(rand() % 11);
    real_time = clock() / (double)CLOCKS_PER_SEC;
    kill(getppid(), SIGUSR1);
    while (1) {
    }
    return 0;
}

void handle_sigusr2(int i){
    printf("PID: %d time difference %f\n", getpid(), (clock() / (double)CLOCKS_PER_SEC) - real_time);
    fflush(stdout);
    kill(getppid(),SIGRTMIN+rand()%32);
    exit(0);
}
