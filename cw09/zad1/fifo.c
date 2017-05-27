#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <memory.h>
#include <thread_db.h>
#include <fcntl.h>
#include <signal.h>
#include "fifo.h"

pthread_mutex_t sem_reader, sem_acq;
pthread_cond_t cond;

int sem_taken, sem_writers;
int* mem;
int div_num, option, readers, writers;
thread_t* writers_t;
thread_t* readers_t;
thread_t* queue;
int queue_length;

void* write_t(void* unused);
void* read_t(void* unused);

void clean_and_exit(int i){
    for (int j = 0; j < writers; j++) {
        pthread_cancel(writers_t[j]);
    }
    for (int j = 0; j < readers; j++) {
        pthread_cancel(readers_t[j]);
    }
    free(writers_t);
    free(readers_t);
    free(queue);
    pthread_mutex_destroy(&sem_reader);
    pthread_mutex_destroy(&sem_acq);
    exit(0);
}

int main (int argc, char** argv)
{
    if(argc != 5){
        printf("./main writers readers divisible option");
        exit(-1);
    }
    signal(SIGINT, &clean_and_exit);
    sem_taken = 0;
    sem_writers = 0;
    mem = (int*)calloc(MEM_SIZE, sizeof(int));

    queue = (thread_t*)calloc(QUEUE_LENGTH, sizeof(thread_t));
    queue_length = 0;
    sem_reader = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    sem_acq = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    cond   = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    writers = atoi(argv[1]);
    readers = atoi(argv[2]);
    div_num = atoi(argv[3]);
    option = atoi(argv[4]);
    writers_t = (thread_t*)calloc(writers, sizeof(thread_t*));
    readers_t = (thread_t*)calloc(readers, sizeof(thread_t*));
    for(int i = 0; i < writers; i++) {
        pthread_create(&writers_t[i], NULL, &write_t, NULL);
    }

    for(int i = 0; i < readers; i++){
        pthread_create(&readers_t[i], NULL, &read_t, NULL);
    }
    for (int i = 0; i < writers; i++){
        pthread_join(writers_t[i], NULL);
    }
    for (int i = 0; i < readers; i++){
        pthread_join(readers_t[i], NULL);
    }
}

void* read_t(void* unused) {
    int j;
    int* indexes = (int*)calloc(MEM_SIZE, sizeof(int));
    int* values = (int*)calloc(MEM_SIZE, sizeof(int));
    thread_t self = pthread_self();
    while(1){
        int count = 0;
        pthread_mutex_lock(&sem_acq);
            while(queue_length >= QUEUE_LENGTH){
                pthread_cond_wait(&cond, &sem_acq);
            }
            queue[queue_length] = self;
            queue_length++;
            while(!pthread_equal(queue[0], self) || sem_writers > 0){
                pthread_cond_wait(&cond, &sem_acq);
            }
            for(int i = 0; i < queue_length; i++){
                queue[i] = queue[i+1];
            }
            queue_length--;
            pthread_cond_broadcast(&cond);
        if(sem_taken > 0){
            sem_taken++;
        }
        else{
            pthread_mutex_lock(&sem_reader);
            sem_taken++;
        }
        pthread_mutex_unlock(&sem_acq);


        j = 0;
        for(int i = 0; i < MEM_SIZE; i++){
            if(mem[i] % div_num == 0){
                indexes[j] = i;
                values[j] = mem[i];
                count++;
                j++;
            }
        }

        pthread_mutex_lock(&sem_acq);
        sem_taken--;
        if(sem_taken == 0){
            pthread_mutex_unlock(&sem_reader);
        }
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&sem_acq);
        printf("\n\nreader %ld found: %d\n", (long int)pthread_self(), count);
        for(int i = 0; i < j && option == 1; i++){
            printf("[%d] = %d ", indexes[i], values[i]);
        }
        printf("\n");
    }
    return NULL;
}

void* write_t(void* unused) {
    srand(time(NULL));
    thread_t self = pthread_self();
    while(1) {
        pthread_mutex_lock(&sem_acq);
            while(queue_length >= QUEUE_LENGTH){
                pthread_cond_wait(&cond, &sem_acq);
            }
            queue[queue_length] = self;
            queue_length++;
            while(!pthread_equal(queue[0], self) || sem_taken > 0){
                pthread_cond_wait(&cond, &sem_acq);
            }
            for(int i = 0; i < queue_length; i++){
                queue[i] = queue[i + 1];
            }
            queue_length--;
            pthread_mutex_lock(&sem_reader);
            sem_writers++;
            pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&sem_acq);

        int size = rand() % MEM_SIZE;
        printf("\n\n=========================\nWriter %ld\n", (long int) pthread_self());
        for(int i = 0; i < size; i++) {
            int index = rand() % MEM_SIZE;
            int number = rand();
            mem[index] = number;
            if(option == 1) printf("[%d] = %d ", index, number);
        }
        printf("\nwriter wrote %d numbers\n=====================\n\n", size);
        sem_writers--;
        pthread_mutex_unlock(&sem_reader);
        pthread_cond_broadcast(&cond);
    }
    return NULL;
}
