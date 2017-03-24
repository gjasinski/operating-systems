#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <bits/time.h>
#include <math.h>

#define LINE_MAX 100
#define BUFFER_SIZE 200

int read_file_to_buffer(int file_desc, char* buff);
int read_line(char* buffer, char* line, int position);
void environment_variable_handler(char* line);
int thread_handler(char* line, unsigned int rlimit_cpu, unsigned int rlimit_as);
int set_limit(int resource, unsigned int value);
void print_usage();


int main(int argc, char* argv[]) {
    if(argc != 4){
        printf("%s\n", "main <batch file name> <max processor time> <max virtual memory in bytes>");
        return 0;
    }
    int batch_file = open(argv[1], O_RDONLY);
    if (batch_file < -1) {
        printf("Can't open file %s\n", argv[1]);
        return 1;
    }
    char* buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    char* line;
    int position, buffered_chars;
    do{
        buffered_chars = read_file_to_buffer(batch_file, buffer);
        position = 0;
        while(position < buffered_chars){
            line = (char*)calloc(LINE_MAX, sizeof(char));
            position = read_line(buffer, line, position);
            if(line[0] == '#') environment_variable_handler(line + 1);
            else if(line[0] != 0) thread_handler(line, (unsigned int)strtol(argv[2], NULL, 10), (unsigned int)strtol(argv[3], NULL, 10) * 1048576);
            free(line);
        }
    }while(buffered_chars == BUFFER_SIZE);
    return 0;
}

int read_file_to_buffer(int file_desc, char* buff){
    int ret_val = (int) read(file_desc, buff, BUFFER_SIZE);
    if(ret_val == -1){
        printf("%s\n", "Can't read file");
        exit(1);
    }
    int i = 1;
    if(ret_val == BUFFER_SIZE){
        while(buff[BUFFER_SIZE - i] != '\n'){
            buff[BUFFER_SIZE - i] = 0;
            i++;
        }
        if((int)lseek(file_desc, -i + 1, SEEK_CUR) == -1){
            printf("%s\n", "Buffer error");
            exit(1);
        }
    }
    return ret_val;
}


int read_line(char* buffer, char* line, int position){
    int i = 0;
    while(buffer[position + i] != '\n' && buffer[position + i] != 0 && i < LINE_MAX){
        i++;
    }
    strncpy(line, buffer + position, i);
    return position + i + 1;
}

void environment_variable_handler(char* line){
    int i = 0;
    while(line[i] != 0 && line[i] != ' '){
        i++;
    }
    if(line[i] == ' '){
        line[i] = 0;
        setenv(line, line + i + 1, 1);
    }
    else{
        unsetenv(line);
    }
}

int thread_handler(char* line, unsigned int rlimit_cpu, unsigned int rlimit_as){
    unsigned int count = 0;
    for(int i = 0; i < strlen(line); i++){
        if (line[i] == ' '){
            count++;
        }
    }
    char** args = (char**)calloc(count + 1, sizeof(char*));
    args[0] = line;
    count = 1;
    for(int i = 0; i < strlen(line) - 1; i++){
        if(line[i] == ' ') {
            line[i] = 0;
            args[count] = line + i + 1;
            count++;
        }
    }
    int return_value;
    int pid = fork();
    if(pid == 0) {
        if(set_limit(RLIMIT_CPU, rlimit_cpu) != 0 || set_limit(RLIMIT_AS, rlimit_as) != 0){
            exit(-1);
        }
        int res = execvp(line, args);
        if(res == -1) abort();
        else exit(res);
    }
    else{
        waitpid(pid, &return_value, 0);
        print_usage();
        if(WIFSIGNALED(return_value)){
            printf("Abnormal termination, command = %s, signal number = %d\n", line, WTERMSIG(return_value));
            exit(return_value);
        }
        if(WIFSTOPPED(return_value)){
            printf("Stopped %d\n, ", WSTOPSIG(return_value));
        }
        if(WIFEXITED(return_value))
            if(WEXITSTATUS(return_value) == 0) return 0;
            else {
                printf("%d\n", WEXITSTATUS(return_value));
                printf("Abnormal exit, command = %s, signal number = %d\n", line, WEXITSTATUS(return_value));
                exit(WEXITSTATUS(return_value));
            }

    }
}

int set_limit(int resource, unsigned int value){
    struct rlimit limit;
    limit.rlim_max = value;
    limit.rlim_cur = value;
    return setrlimit(resource, &limit);

}

void print_usage(){
    struct rusage s_rusage;
    getrusage(RUSAGE_CHILDREN, &s_rusage);
    printf("Times: user = %f, system %f\n", s_rusage.ru_utime.tv_sec + s_rusage.ru_utime.tv_usec/pow(10.0, 6.0), s_rusage.ru_stime.tv_sec + s_rusage.ru_stime.tv_usec/pow(10.0, 6.0));
}