#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>

int signal_received;

void set_sig_handler();

int main(){
    printf("OOO");
    fflush(stdout);
    signal_received = 0;
    set_sig_handler();
    while(1){
    }
}

void handle_sig(int sig, siginfo_t *siginfo, void *context){
    printf("signal\n");
    fflush(stdout);
    signal_received++;
    kill(getppid(), sig);
}

void handle_sig_kill(int sig, siginfo_t *siginfo, void *context){
    printf("Child - signal received: %d", signal_received);
    fflush(stdout);
    exit(0);
}


void set_sig_handler() {
    struct sigaction act;
    printf("ALA MA KOTA");
    act.sa_sigaction = &handle_sig;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGRTMIN, &act, NULL);

    act.sa_sigaction = &handle_sig_kill;
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGRTMIN + 1, &act, NULL);
}