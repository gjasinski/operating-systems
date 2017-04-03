#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

double real_time;

void handle_sigusr2(int i);

int main() {
    signal(SIGUSR2, &handle_sigusr2);
    srand(time(NULL));
    sleep(rand() % 11);
    real_time = clock() / (double)CLOCKS_PER_SEC;
    /*printf("przed wyslaniem\n");
    fflush(stdout);*/
    kill(getppid(), SIGUSR1);
    printf("wyslalem sig\n");
    fflush(stdout);
    while (1) {
        sleep(1);
    }
    return 0;
}

void handle_sigusr2(int i){
    printf("Odebralem singal\n");
    fflush(stdout);
    //kill(getppid(),SIGRTMIN+random()%32);
    printf("Time difference %f\n", (clock() / (double)CLOCKS_PER_SEC) - real_time);
    fflush(stdout);
    exit(0);
}
