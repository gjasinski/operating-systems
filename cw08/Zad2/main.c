#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 700
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


#define RECORD_SIZE 1024

int thread_amount;
int option;
char* file_name;
int record_number;
int signal_arg;
char* searched_word;
FILE* file_ptr;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_init = PTHREAD_MUTEX_INITIALIZER;
pthread_t* threads;
pthread_key_t buffer_key;
char* buffer;

void asynchronous_threads();
void synchronous_threads();
void detached_threads();

void handle_sigusr1(int i){
  printf("sigusr1 PID %d TID %ld\n", getpid(), pthread_self());
  exit(0);
}


void handle_sigterm(int i){
  printf("sigterm PID %d TID %ld\n", getpid(), pthread_self());
  exit(0);
}


void handle_sigkill(int i){
  printf("sigkill PID %d TID %ld\n", getpid(), pthread_self());
  exit(0);
}

void handle_sigstop(int i){
  printf("sigstop PID %d TID %ld\n", getpid(), pthread_self());
  exit(0);
}

void set_sigtstp_handler() {
    signal(SIGUSR1, &handle_sigusr1);
    signal(SIGTERM, &handle_sigterm);
    signal(SIGKILL, &handle_sigkill);
    signal(SIGSTOP, &handle_sigstop);
}

int main (int argc, char** argv)
{
set_sigtstp_handler();
    /*sigset_t * new_set;
    sigemptyset(new_set);
    sigaddset(new_set, SIGUSR1);
    sigaddset(new_set, SIGTERM);
    sigaddset(new_set, SIGKILL);
    sigaddset(new_set, SIGSTOP);
    sigprocmask(SIG_BLOCK, new_set, NULL);*/
    sigset_t * new_set;
    sigemptyset(new_set);
    sigaddset(new_set, SIGUSR1);
    sigaddset(new_set, SIGTERM);
    sigprocmask(SIG_BLOCK, new_set, NULL);
    if(argc != 4){
        printf("./main option thread_amount\n");
        exit(-1);
    }
    option = atoi(argv[1]);
    thread_amount = atoi(argv[2]);
    signal_arg = atoi(argv[3]);
    if(option == 1) asynchronous_threads();
    if(option == 2) synchronous_threads();
    if(option == 3) detached_threads();
    return 0;
}

void* asynchronous_read(void* unused){
/*  sigset_t * new_set;
  sigemptyset(new_set);
  sigaddset(new_set, SIGUSR1);
  sigaddset(new_set, SIGTERM);
  sigprocmask(SIG_BLOCK, new_set, NULL);*/
    //set_sigtstp_handler();
    buffer = (char*)calloc(record_number * (RECORD_SIZE + 4), sizeof(char));
    /*pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setspecific(buffer_key, buffer);
    pthread_mutex_lock(&mutex_init);
    pthread_mutex_unlock(&mutex_init);
*/
    int end_of_reading = 0;

    if(pthread_equal(pthread_self(), threads[1])){
      int a = 0;
      int b = 3;
      int c = b /a;
      printf("%d\n", c);
    }
    while(1){
        printf("Thread %ld\n\n", (long int)pthread_self());
        sleep(1);
    }
    return NULL;
}

void* synchronous_read(void* unused){
    buffer = (char*)calloc(record_number * (RECORD_SIZE + 4), sizeof(char));
    pthread_setspecific(buffer_key, buffer);
    pthread_mutex_lock(&mutex_init);
    pthread_mutex_unlock(&mutex_init);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    pthread_testcancel();

    while(1){
        printf("Thread %ld\n\n", (long int)pthread_self());
        sleep(1);
    }

    fflush(stdout);
    pthread_exit(0);
}

void* detached_read(void* unused){
    pthread_setspecific(buffer_key, buffer);
    buffer = (char*)calloc(record_number * (RECORD_SIZE + 4), sizeof(char));
    pthread_mutex_lock(&mutex_init);
    pthread_mutex_unlock(&mutex_init);

    while(1){
        printf("Thread %ld\n\n", (long int)pthread_self());
        sleep(1);
    }
    return NULL;
}

void free_buffer(void* buffer) {
    free(buffer);
}

void asynchronous_threads(){
    pthread_attr_t attr;
    threads = (pthread_t*)calloc(thread_amount, sizeof(pthread_t));
    pthread_key_create(&buffer_key, free_buffer);
    pthread_mutex_lock(&mutex_init);

    for (int i = 0; i < thread_amount; i++){
        pthread_create(&threads[i], NULL, &asynchronous_read, NULL);
    }
    pthread_mutex_unlock(&mutex_init);
    //if(signal_arg == 1) pthread_kill(threads[1], SIGUSR1);
    //if(signal_arg == 2) pthread_kill(threads[1], SIGTERM);
  //  if(signal_arg == 3) pthread_kill(threads[1], SIGKILL);
    //if(signal_arg == 4) pthread_kill(threads[1], SIGSTOP);

    for (int i = 0; i < thread_amount; i++){
        pthread_join(threads[i], NULL);
    }
};

void synchronous_threads(){
    pthread_attr_t attr;
    threads = (pthread_t*)calloc(thread_amount, sizeof(pthread_t));
    pthread_key_create(&buffer_key, free_buffer);
    pthread_mutex_lock(&mutex_init);
    for (int i = 0; i < thread_amount; i++){
        pthread_create(&threads[i], NULL, &synchronous_read, NULL);
    }
    pthread_mutex_unlock(&mutex_init);

    for (int i = 0; i < thread_amount; i++){
        pthread_join(threads[i], NULL);
        printf("%d\n", i);
        fflush(stdout);
    }
};

void detached_threads(){
    pthread_attr_t attr;
    threads = (pthread_t*)calloc(thread_amount, sizeof(pthread_t));
    pthread_key_create(&buffer_key, free_buffer);
    pthread_mutex_lock(&mutex_init);
    for (int i = 0; i < thread_amount; i++){
        pthread_create(&threads[i], NULL, &detached_read, NULL);
        pthread_detach(threads[i]);
    }
    pthread_mutex_unlock(&mutex_init);
        for (int i = 0; i < thread_amount; i++){
        pthread_join(threads[i], NULL);
    }
    while(1){};
};
