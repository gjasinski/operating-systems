#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int status;

void set_sigtstp_handler() ;

void set_sigint_handler();

int main() {
    status = 0;
    char c = 'Z';
    set_sigtstp_handler();
    set_sigint_handler();
    while(1){
        if(status == 0){
            if(c == 'Z'){
                c = 'A';
            }
            else{
                c++;
            }
        }
        else{
            if(c == 'A'){
                c = 'Z';
            }
            else{
                c--;
            }
        }
        printf("%c", c);
        fflush(stdout);
        sleep(1);
    }
    return 0;
}

void handle_sigint(int sig, siginfo_t *siginfo, void *context){
    printf("%s", "\nOdebrano sygnał SIGINT\n");
    siginfo = context = NULL;
    exit(sig);
}

void set_sigint_handler() {
    struct sigaction act;
    act.sa_sigaction = &handle_sigint;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &act, NULL);
}


void handle_sigtstp(int i){
    status++;
    status = status % 2;
    set_sigtstp_handler();
    i++;
}

void set_sigtstp_handler() {
    signal(SIGTSTP, &handle_sigtstp);
}