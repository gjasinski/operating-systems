#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

int* children_array;
int* children_signaled_array;
int children_signaled;
int children_created;
int M;
int odebrane;

void set_sigusr1_handler() ;

void print_signal_info(int pid, int signal);

void set_sigusr2_handler() ;

void create_childrens(int N);

void set_sigint_handler() ;

int main(int argc, char* argv[]) {
    odebrane=0;
    if(argc != 3){
        printf("%s\n", "main <N - childrens> <M - start approve>");
        return 0;
    }
    int N = atoi(argv[1]);
    M = atoi(argv[2]);
    children_signaled = children_created = 0;
    children_array = (int*)calloc(N, sizeof(int));
    children_signaled_array = (int*)calloc(N, sizeof(int));
    set_sigint_handler();
    set_sigusr1_handler();
    set_sigusr2_handler();
    create_childrens(N);

    int return_value;
    for(int i = 0; i< N; i++){
        waitpid(children_array[i], &return_value, 0);
        printf("--%d %d\n",i, return_value);
    }
    return 0;
}

void create_childrens(int N){
    for(children_created; children_created < N; children_created++){
        int pid = fork();
        if(pid == 0){
            int res = execvp("./child", NULL);
            if(res == -1){
                printf("%s", "execvp error\n");
                exit(0);
            }
        }
        else{
            children_array[children_created] = pid;
        }
    }
}


void handle_sigusr1(int sig, siginfo_t *siginfo, void *context){
    odebrane++;
    children_signaled_array[children_signaled] = siginfo->si_pid;
    children_signaled++;
    if(children_signaled == M){
        printf("\n\n=======$$%^&^%$%^&*&^%$\n\n");
        for(int i = 0; i < children_signaled; i++){
            kill(children_signaled_array[i], SIGUSR2);
        }
    }
    if(children_signaled > M){
        kill(children_signaled_array[children_signaled - 1], SIGUSR2);
    }
    set_sigusr1_handler();
    printf("YOLOODEBRALEM   %d\n", odebrane);
    fflush(stdout);
//    print_signal_info(children_signaled_array[children_signaled - 1], sig);
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
    print_signal_info(siginfo->si_pid, sig);
    set_sigusr2_handler();
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

void set_sigint_handler() {
    struct sigaction act;
    act.sa_sigaction = &handle_sigint;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &act, NULL);

}


void print_signal_info(int pid, int signal){
    printf("Child: %d - signal: %d", pid, signal);
}
