#define _BSD_SOURCE || _XOPEN_SOURCE >= 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

int signal_sended, signal_received, child_pid, T;

void send_signal();
void send_kill_signal();
void set_sig_handler();

int main(int argc, char* argv[]) {
    if(argc != 3){
        printf("%s\n", "main <L - signals> <T - type>");
        return 0;
    }
    int L = atoi(argv[1]);
    T = atoi(argv[2]);
    signal_received = signal_sended = 0;
    set_sig_handler();
    int pid = fork();
    child_pid = pid;
    char** arg = (char**)calloc(0, sizeof(char*));
    if(pid == 0){
        int res = execvp("./child", arg);
        if(res == -1){
            printf("%s", "execvp error\n");
            exit(0);
        }
    }
    sleep(1);
    for(int i = 0; i < L; i++){
        //usleep(5000);
        send_signal();
      //  usleep(5000);
    }
    send_kill_signal();
    int return_value;
    waitpid(pid, &return_value, 0);
  //  sleep(4);
    printf("%s %d\n%s %d \n", "Signal sended: ", signal_sended, "Signal received: ", signal_received);
    fflush(stdout);

    return 0;
}

void send_signal(){
    signal_sended++;
    if(T == 1){
        kill(child_pid, SIGUSR1);
    }
    if(T == 2){
        union sigval val;
        val.sival_int = SIGUSR1;
        if(sigqueue(child_pid, SIGUSR1, val) != 0){
            printf("%s \n", "sigqueue - error");
            fflush(stdout);
        }
    }
    if(T == 3){
        kill(child_pid, SIGRTMIN);
    }
}

void send_kill_signal(){
    if(T == 1){
        kill(child_pid, SIGUSR2);
    }
    if(T == 2){
        union sigval val;
        val.sival_int = SIGUSR2;
        if(sigqueue(child_pid, SIGUSR2, val) != 0){
            printf("%s \n", "sigqueue - error");
            fflush(stdout);
        }
    }
    if(T == 3){
        kill(child_pid, SIGRTMIN + 1);
    }
}

void handle_sigusr1(int sig, siginfo_t *siginfo, void *context){
    signal_received++;
}

void handle_sigint(int sig, siginfo_t *siginfo, void *context){
    if(T == 1 || T == 3){
        kill(child_pid, SIGINT);
    }
    else{
        union sigval val;
        val.sival_int = SIGINT;
        if(sigqueue(child_pid, SIGINT, val) != 0){
            printf("%s \n", "sigqueue - error");
            fflush(stdout);
        }
    }
    exit(SIGINT);
}

void set_sig_handler() {
    struct sigaction act;
    act.sa_sigaction = &handle_sigusr1;
    act.sa_flags = SA_SIGINFO;
    if(T == 1 || T == 2){
        sigaction(SIGUSR1, &act, NULL);
    }
    else{
        sigaction(SIGRTMIN, &act, NULL);
    }
    act.sa_sigaction = &handle_sigint;
    sigaction(SIGINT, &act, NULL);
}
