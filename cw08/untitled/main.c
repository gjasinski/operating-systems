#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#define RECORD_SIZE 1024

int thread_amount;
int option;
char* file_name;
int record_number;
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

int main (int argc, char** argv)
{
    if(argc != 6){
        printf("./main option thread_amount file_name record_number searched_word\n");
        exit(-1);
    }
    option = atoi(argv[1]);
    thread_amount = atoi(argv[2]);
    file_name = argv[3];
    record_number = atoi(argv[4]);
    searched_word = argv[5];

    file_ptr = fopen(file_name, "r");
    if(file_ptr == NULL){
        printf("Couldn't open file %s\n", strerror(errno));
    }

    if(option == 1) asynchronous_threads();
    if(option == 2) synchronous_threads();
    if(option == 3) detached_threads();
    return 0;
}
int search_words(){
    int end_of_reading = 0;
    int position = ftell(file_ptr) / 1028;
    size_t read_chars = fread(buffer, RECORD_SIZE + 4, record_number, file_ptr);

    if(read_chars > 0){
        for(int i = 0; i < record_number && end_of_reading == 0; i++){
            char* searched_str = strstr(buffer + 4, searched_word);
            if(searched_str != NULL) {
                end_of_reading = 1;
                printf("Thread %d found \"%s\" at position %d\n", (int)pthread_self(), searched_word, position + i);
                for (int j = 0; j < thread_amount && option != 3; j++) {
                    pthread_t self = pthread_self();
                    if (!pthread_equal(self, threads[j])) {
                        pthread_cancel(threads[j]);
                        exit(0);
                    }
                }
                fflush(stdout);
                return end_of_reading;
            }
        }
    }
    return end_of_reading;
}
void* asynchronous_read(void* unused){
    buffer = (char*)calloc(record_number * (RECORD_SIZE + 4), sizeof(char));
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setspecific(buffer_key, buffer);
    pthread_mutex_lock(&mutex_init);
    pthread_mutex_unlock(&mutex_init);

    int end_of_reading = 0;
    while(end_of_reading == 0){
        pthread_mutex_lock(&mutex);
        end_of_reading = search_words();
        pthread_mutex_unlock(&mutex);
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

    int end_of_reading = 0;

    pthread_testcancel();

    while(end_of_reading == 0){
        pthread_testcancel();
        pthread_mutex_lock(&mutex);
        end_of_reading = search_words();
        pthread_mutex_unlock(&mutex);
        pthread_testcancel();
    }

    fflush(stdout);
    pthread_exit(0);
}

void* detached_read(void* unused){
    pthread_setspecific(buffer_key, buffer);
    buffer = (char*)calloc(record_number * (RECORD_SIZE + 4), sizeof(char));
    pthread_mutex_lock(&mutex_init);
    pthread_mutex_unlock(&mutex_init);

    int end_of_reading = 0;
    while(end_of_reading == 0){
        pthread_mutex_lock(&mutex);
        end_of_reading = search_words();
        pthread_mutex_unlock(&mutex);
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
    sleep(2);
};
