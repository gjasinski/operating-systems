#include "common.h"

int chairs;
int shm_desc_q, shm_desc_s;
int* shm_memory_q;
int* shm_memory_s;
sem_t* sem_barber;
sem_t* sem_queue;
sem_t* sem_barber_sleeping;
sem_t* sem_barber_cutting;
sem_t** waiting_room;
void set_up_common_memory(int chairs);

void clean_and_exit(int i);

void set_up_semaphores();

void barber_loop();

int main(int argc, char* argv[]){
    if (argc != 2) {
        printf("Wrong arguments - barber");
        return 0;
    }
    chairs = atoi(argv[1]);
    set_up_common_memory(chairs);
    signal(SIGINT, &clean_and_exit);
    set_up_semaphores();
    barber_loop();
}

void print_info(char* info, int pid){
    struct timespec tp;
    if(clock_gettime(CLOCK_MONOTONIC, &tp) == -1) printf("error");
    struct tm* time = localtime(&tp.tv_sec);
    char* result = (char*)calloc(20, sizeof(char));
    sprintf(result, "%d:%d:%d:%ld", time->tm_hour, time->tm_min, time->tm_sec, tp.tv_nsec/1000);
    printf("Barber - %s %d- %s\n", info, pid, result);
    free(result);
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

void set_up_common_memory(int chairs){
    shm_desc_q = shm_open(SHM_NAME_QUEUE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP);
    shm_desc_s = shm_open(SHM_NAME_SEMAPHORES, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP);
    if (shm_desc_q == -1 || shm_desc_s == -1) printf("set_up_common_memory %s\n", strerror(errno));
    if (ftruncate(shm_desc_q, SHM_SIZE) != 0 || ftruncate(shm_desc_s, SHM_SIZE)) printf("set_up_common_memory %s\n", strerror(errno));
    shm_memory_q =(int*) mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc_q, 0);
    shm_memory_s =(int*) mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc_s, 0);
    if (shm_memory_q == MAP_FAILED || shm_memory_s == MAP_FAILED) printf("set_up_common_memory %s\n", strerror(errno));
    shm_memory_q[0] = getpid();   //SHM_BARBER_PID
    shm_memory_q[1] = chairs;     //SHM_CHAIRS
    shm_memory_q[2] = -1;         //SHM_WAKING_CLIENT
    shm_memory_q[3] = 3;          //SHM_QUEUE_END
    //0 - barber_pid;
    //1 - chairs;
    //2 - queue_end;
    //3 - waking client
    //4 - first client's pid
}

void clean_and_exit(int i){
    for(int i = 0; i < SHM_SIZE; i++){
        if (sem_close(waiting_room[i]) == -1) printf("clean_and_exit %s\n", strerror(errno));
    }
    if (sem_close(sem_barber) == -1 || sem_close(sem_queue)) printf("clean_and_exit %s\n", strerror(errno));
    if (munmap(shm_memory_q, SHM_SIZE) == -1 || munmap(shm_memory_s, SHM_SIZE) == -1) printf("clean_and_exit %s\n", strerror(errno));
    if (shm_unlink(SHM_NAME_QUEUE) == -1 || shm_unlink(SHM_NAME_SEMAPHORES) == -1) printf("clean_and_exit %s\n", strerror(errno));
    exit(EXIT_SUCCESS);
}

void set_up_semaphores(){
    sem_barber = sem_open(SEM_BARBER, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);
    sem_queue = sem_open(SEM_QUEUE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);
    sem_barber_sleeping = sem_open(SEM_BARBER_SLEEPING, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);
    sem_barber_cutting = sem_open(SEM_CUTTING, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);

    waiting_room = (sem_t**)calloc(SHM_SIZE, sizeof(sem_t*));

    for(int i = 0; i < SHM_SIZE; i++){
        shm_memory_s[i] = i;
        char* tmp = (char*)calloc(5, sizeof(char));
        sprintf(tmp, "/%d", i);
        waiting_room[i] = sem_open(tmp, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);
        if(waiting_room[i] == SEM_FAILED){printf("ERROR sem_waiting_room %s\n", strerror(errno));}
        while(get_semaphore(waiting_room[i], SEM_NOWAIT) != -1){
        }
        free(tmp);
    }
    if(sem_barber == SEM_FAILED || sem_queue == SEM_FAILED || sem_barber_cutting == SEM_FAILED || sem_barber_sleeping == SEM_FAILED) printf("set_up_semaphores %s\n", strerror(errno));
    while(get_semaphore(sem_barber, SEM_NOWAIT) != -1);
    while(get_semaphore(sem_queue, SEM_NOWAIT) != -1);
    while(get_semaphore(sem_barber_sleeping, SEM_NOWAIT) != -1);
    while(get_semaphore(sem_barber_cutting, SEM_NOWAIT) != -1);
    release_semaphore(sem_queue);
}

int get_next_client(){
    if(shm_memory_q[SHM_QUEUE_END] > 3){
        return shm_memory_q[4];
    }
    else return -1;
}

void barber_loop(){
    while(1){
        get_semaphore(sem_queue, SEM_WAIT);
        int res = get_next_client();
        if(res == -1){
            release_semaphore(sem_barber);
            release_semaphore(sem_queue);
            print_info("goes to sleep", 0);
            get_semaphore(sem_barber_sleeping, SEM_WAIT);
            int pid = shm_memory_q[SHM_WAKING_CLIENT];
            print_info("start cutting", pid);
            print_info("finished cutting", pid);
            release_semaphore(sem_barber_cutting);
        }
        else{
            release_semaphore(sem_queue);
            print_info("start cutting", res);
            print_info("finished cutting", res);
            release_semaphore(waiting_room[shm_memory_s[0]]);

            get_semaphore(sem_queue, SEM_WAIT);
            int tmp = shm_memory_s[0];
            for(int i = 0; i < SHM_SIZE - 1; i++){
                shm_memory_s[i] = shm_memory_s[i+1];
            }
            shm_memory_s[SHM_SIZE - 1] = tmp;
            int tmp2 = shm_memory_q[4];
            for(int i = 4; i < SHM_SIZE - 1; i++){
                shm_memory_q[i] = shm_memory_q[i + 1];
            }
            shm_memory_q[SHM_SIZE - 1] = tmp2;
            shm_memory_q[SHM_QUEUE_END]--;
            release_semaphore(sem_queue);
        }
    }
}
