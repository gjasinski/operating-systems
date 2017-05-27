#include "common.h"
int cuts;
int shm_desc_q, shm_desc_s;
int* shm_memory_q;
int* shm_memory_s;
sem_t* sem_barber;
sem_t* sem_queue;
sem_t* sem_barber_sleeping;
sem_t* sem_barber_cutting;
sem_t** waiting_room;
void get_shm();
void client_loop();

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Wrong arguments - client");
        return 0;
    }

    cuts = atoi(argv[1]);
    get_shm();

    sem_barber = sem_open(SEM_BARBER, 0);
    sem_queue = sem_open(SEM_QUEUE, 0);
    sem_barber_sleeping = sem_open(SEM_BARBER_SLEEPING, 0);
    sem_barber_cutting = sem_open(SEM_CUTTING, 0);
    if(sem_barber == SEM_FAILED || sem_queue == SEM_FAILED || sem_barber_cutting == SEM_FAILED || sem_barber_sleeping == SEM_FAILED) printf("set_up_semaphores %s\n", strerror(errno));
    waiting_room = (sem_t**)calloc(SHM_SIZE, sizeof(sem_t*));
    for(int i = 0; i < SHM_SIZE; i++){
      char* sem_name = (char*)calloc(5, sizeof(char));
      sprintf(sem_name, "/%d", i);
      waiting_room[i] = sem_open(sem_name, 0);
      free(sem_name);
    }
    client_loop();
    return 0;
}

int get_semaphore(sem_t* sem, int block){
    if (block == 1){
        if (sem_wait(sem) == -1) {
            printf("get_semaphore %s", strerror(errno));
            return -1;
        }
        else{
            return 0;
        }
    }
    else{
        return sem_trywait(sem);
    }
}

void release_semaphore(sem_t* sem){
    if (sem_post(sem) == -1) printf("release_semaphore %s", strerror(errno));
}
void get_shm(){
    shm_desc_q = shm_open(SHM_NAME_QUEUE, O_RDWR, S_IRUSR | S_IWUSR | S_IWGRP);
    shm_desc_s = shm_open(SHM_NAME_SEMAPHORES, O_RDWR, S_IRUSR | S_IWUSR | S_IWGRP);
    if (shm_desc_q == -1 || shm_desc_s == -1) printf("get_shm %s\n", strerror(errno));
    shm_memory_q =(int*) mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc_q, 0);
    shm_memory_s =(int*) mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc_s, 0);
    if (shm_memory_q == MAP_FAILED || shm_memory_s == MAP_FAILED) printf("set_up_common_memory %s\n", strerror(errno));
}

void print_info(char* info, int pid){
    struct timespec tp;
    if(clock_gettime(CLOCK_MONOTONIC, &tp) == -1) printf("error");
    struct tm* time = localtime(&tp.tv_sec);
    char* result = (char*)calloc(20, sizeof(char));
    sprintf(result, "%d:%d:%d:%ld", time->tm_hour, time->tm_min, time->tm_sec, tp.tv_nsec/1000);
    printf("Barber - %s %d- %s\n", info, pid, result);
    free(result);
    fflush(stdout);
}

/*void open_get_and_release_semaphore(int sem_id){
    char* sem_name = (char*)calloc(5, sizeof(char));
    sprintf(sem_name, "/%d", sem_id);
    printf("uwalniam %s\n", sem_name);
    sem_t* sem = sem_open(sem_name, 0);
    get_semaphore(sem, SEM_WAIT);
    free(sem_name);
    sem_close(sem);
}*/
void client_loop(){
    while(1){
        if(cuts == 0) exit(0);
        get_semaphore(sem_queue, SEM_WAIT);
        if(get_semaphore(sem_barber, IPC_NOWAIT) != -1){
            shm_memory_q[SHM_WAKING_CLIENT] = getpid();
            release_semaphore(sem_barber_sleeping);
            release_semaphore(sem_queue);
            print_info("waking barber", getpid());
            get_semaphore(sem_barber_cutting, SEM_WAIT);
            print_info("client left barber after cutting", getpid());
            cuts--;
        }
        else{
            if(shm_memory_q[SHM_QUEUE_END] > 3 + shm_memory_q[SHM_CHAIRS]){
                release_semaphore(sem_queue);
                print_info("client left barber - no free seats", getpid());
            }
            else{
                shm_memory_q[shm_memory_q[SHM_QUEUE_END] + 1] = getpid();
                int tmp = shm_memory_s[shm_memory_q[SHM_QUEUE_END] - 3];
                shm_memory_q[SHM_QUEUE_END]++;
                release_semaphore(sem_queue);
                print_info("sit in waiting room", getpid());
              
                get_semaphore(waiting_room[tmp], SEM_WAIT);
                print_info("client left barber after cutting", getpid());
                cuts--;
            }
        }
    }
}
