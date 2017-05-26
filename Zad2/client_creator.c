
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Wrong arguments - client_creator");
        return 0;
    }
    int clients = atol(argv[1]);
    int cuts= atol(argv[2]);
    int* clients_pid = (int*)calloc(clients, sizeof(int));
    char* arg[3];
    arg[0] = "./client";
    arg[1] = argv[2];
    arg[2] = NULL;
    for (int i = 0; i < clients; i++){
        pid_t pid = fork();
        if(pid == 0){
            if(execvp("./client", arg) == -1){
                printf("ERROR");
            }
        }
        clients_pid[i] = pid;
    }
    int return_value;
    for(int i = 0; i < clients; i++){
        waitpid(clients_pid[i], &return_value, 0);
    }
}