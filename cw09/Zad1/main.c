#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <memory.h>
#include <thread_db.h>
#include <fcntl.h>
#include <signal.h>
#include "main.h"

sem_t* sem_reader;
sem_t* sem_acq;

int sem_taken;
int* mem;
int div_num, option, readers, writers;
thread_t* writers_t;
thread_t* readers_t;
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
    sem_destroy(sem_reader);
    sem_destroy(sem_acq);
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
    mem = (int*)calloc(MEM_SIZE, sizeof(int));

    sem_reader = (sem_t*)calloc(1, sizeof(sem_t));
    sem_acq = (sem_t*)calloc(1, sizeof(sem_t));
    sem_init(sem_reader, 0, 1);
    sem_init(sem_acq, 0, 1);

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
    while(1){
        int count = 0;
        sem_wait(sem_acq);
        if(sem_taken > 0){
            sem_taken++;
        }
        else{
            sem_wait(sem_reader);
            sem_taken++;
        }
        sem_post(sem_acq);

        j = 0;
        for(int i = 0; i < MEM_SIZE; i++){
            if(mem[i] % div_num == 0){
                indexes[j] = i;
                values[j] = mem[i];
                count++;
                j++;
            }
        }

        printf("\n\nreader %ld found: %d\n", (long int)pthread_self(), count);
        if(option == 1)for(int i = 0; i < j; i++){
            printf("[%d] = %d ", indexes[i], values[i]);
        }
        printf("\n");

        sem_wait(sem_acq);
        sem_taken--;
        if(sem_taken == 0){
            sem_post(sem_reader);
        }

        sem_post(sem_acq);

    }
    return NULL;
}

void* write_t(void* unused) {
    srand(time(NULL));
    while(1) {
        sem_wait(sem_reader);
        int size = rand() % MEM_SIZE;
        printf("\n\n=========================\nWriter %ld\n", (long int) pthread_self());
        for(int i = 0; i < size; i++) {
            int index = rand() % MEM_SIZE;
            int number = rand();
            mem[index] = number;
            if(option == 1) printf("[%d] = %d ", index, number);
        }
        printf("\nwriter wrote %d numbers\n=====================\n\n", size);

        sem_post(sem_reader);
    }
    return NULL;
}
