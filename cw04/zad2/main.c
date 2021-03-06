#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>

int* children_array;
int* children_signaled_array;
int children_signaled;
int children_created;
int M;
int cont;

void set_sigusr1_handler() ;
void set_sigusr2_handler() ;
void create_childrens(int N);
void set_sig_handler() ;

int main(int argc, char* argv[]) {
    if(argc != 3){
        printf("%s\n", "main <N - childrens> <M - start approve>");
        return 0;
    }
    int N = atoi(argv[1]);
    M = atoi(argv[2]);
    children_signaled = children_created = 0;
    children_array = (int*)calloc(N, sizeof(int));
    children_signaled_array = (int*)calloc(N, sizeof(int));
    set_sig_handler();
    set_sigusr1_handler();
    set_sigusr2_handler();
    create_childrens(N);

    int return_value;
    for(int i = 0; i< N; i++){
        waitpid(children_array[i], &return_value, 0);
        printf("Process finished:%d  code:%d\n",children_array[i], return_value);
    }
    return 0;
}

void create_childrens(int N){
    for(children_created = 0; children_created < N; children_created++){
        int pid = fork();
        char** arg = (char**)calloc(0, sizeof(char*));
        if(pid == 0){
            int res = execvp("./child", arg);
            if(res == -1){
                printf("%s", "execvp error\n");
                exit(0);
            }
        }
        else{
            children_array[children_created] = pid;
            cont = 0;
            while(cont == 0){
            }
        }
    }
}


void handle_sigusr1(int sig, siginfo_t *siginfo, void *context){
    children_signaled_array[children_signaled] = siginfo->si_pid;
    printf("Received signal: %d from %d\n", sig, children_signaled_array[children_signaled]);
    fflush(stdout);
    children_signaled++;
    if(children_signaled == M){
        for(int i = 0; i < children_signaled; i++){
            kill(children_signaled_array[i], SIGUSR2);
        }
    }
    if(children_signaled > M){
        kill(children_signaled_array[children_signaled - 1], SIGUSR2);
    }
    cont  = 1;
}

void set_sigusr1_handler() {
    struct sigaction act;
    act.sa_sigaction = &handle_sigusr1;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SA_NODEFER);
    sigaction(SIGUSR1, &act, NULL);
}

void handle_sigusr2(int sig, siginfo_t *siginfo, void *context){
    printf("Received signal: %d from %d\n", sig, siginfo->si_pid);
    fflush(stdout);
}

void handle_sigreal(int sig, siginfo_t *siginfo, void *context){
    printf("Received signal: %d from %d\n", sig, siginfo->si_pid);
    fflush(stdout);
}

void set_sigusr2_handler() {
    struct sigaction act;
    act.sa_sigaction = &handle_sigusr2;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &act, NULL);

}

void handle_sigint(int sig, siginfo_t *siginfo, void *context){
    for(int i = 0; i < children_created; i++){
        kill(children_array[i], SIGINT);
    }
    exit(9);
}

void set_sig_handler() {
    struct sigaction act;
    act.sa_sigaction = &handle_sigint;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &act, NULL);
    act.sa_sigaction = &handle_sigreal;
    for(int i = 0; i < 33; i++){
      sigaction(SIGRTMIN + i, &act, NULL);
    }
}
