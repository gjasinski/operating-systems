#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#define MAX_ARGS 10
#define MAX_INPUT 1000
#define MAX_PIPES 30

void create_pipe(int in, int out, char** args, int not_last);
void input_parser(char* input, char*** args);

int main() {
    char* input = (char *) calloc(MAX_INPUT, sizeof(char));
    char*** args;
    fgets(input, MAX_INPUT, stdin);

    while(input[0] != '\n'){
        args = (char ***) calloc(MAX_PIPES, sizeof(char **));
        input_parser(input, args);

        int fd[2];
        int in = STDIN_FILENO;
        int i;
        for (i = 0; args[i]; i++) {
            pipe(fd);
            if (args[i + 1]) create_pipe(in, fd[1], args[i], 1);
            else create_pipe(in, fd[1], args[i], 0);
            close(fd[1]);
            in = fd[0];
        }

        free(args);
        free(input);
        input = (char *) calloc(MAX_INPUT, sizeof(char));
        fgets(input, MAX_INPUT, stdin);
    }
    return 0;
}


void input_parser(char* input, char*** args){
    for(int i = 0; input[i]; i++){
        if(input[i] == '\n') input[i] = 0;
    }
    int curr_pipe = 0;
    int curr_arg = 1;
    args[0] = (char**)calloc(MAX_ARGS, sizeof(char*));
    args[0][0] = input;

    for(int i = 0; input[i] != '\0'; i++){
        if(input[i] == '|') {
            curr_pipe++;
            curr_arg = 1;
            args[curr_pipe] = (char**)calloc(MAX_ARGS + 1, sizeof(char*));
            if(input[i+1] == ' '){
                input[i] = 0;
                args[curr_pipe][0] = input + i + 2;
                i++;
            }
            else {
                args[curr_pipe][0] = input + i + 1;
            }
            input[i] = 0;
            continue;
        }
        if(input[i] == ' '){
            if(input[i+1] == '|'){
                input[i] = 0;
                continue;
            }
            if(input[i+1] == 0){
                input[i] = 0;
                continue;
            }
            args[curr_pipe][curr_arg] = input + i + 1;
            input[i] = 0;
            curr_arg++;
        }

    }
}

void create_pipe(int in, int out, char** args, int not_last){
    pid_t pid = fork();
    if(pid == 0){
        dup2(in, STDIN_FILENO);
        close(in);
        if(not_last){
            dup2(out, STDOUT_FILENO);
            close(out);
        }
        if(execvp(args[0], args) == -1){
            printf("execvp - err");
            exit(1);
        }
    }
    else{
        int ret;
        waitpid(pid, &ret, 0);
    }
}