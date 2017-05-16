#include "common.h"
#include "string.h"

int chairs;
int shm_desc;
int *shm_memory;
sem_t* sem_barber;
sem_t* sem_barber_walking;
sem_t* sem_barber_sleeping;
sem_t* sem_baber_cutting;
sem_t** sem_waiting_room;

char* SEM_CUTTING  = "/sem_cutting";

void release_semaphore(sem_t* sem);

void set_up_common_memory(int chairs);

void clean_and_exit(int i);

void barber_checks_waiting_room();

void set_up_semaphores();

void barber_cut_client(int client_pid);

//int set_up_receiving_signals();

int get_semaphore(sem_t* sem, int block);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Wrong arguments - barber");
        return 0;
    }
    chairs = atol(argv[1]);
    set_up_common_memory(chairs);
    //set_up_receiving_signals();
    set_up_semaphores();
    barber_checks_waiting_room();
    while(1){}
}

/*
 * In debugging purpose
void print_sem_value(sem_t* sem){
    int info;
    sem_getvalue(sem, &info);
    printf("%s\n", strerror(errno));
    printf("semafor value: %i\n", info);
}
*/

void set_up_common_memory(int chairs){
    shm_desc = shm_open(SHM_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP);
    if (shm_desc == -1) printf("set_up_common_memory %s\n", strerror(errno));
    if (ftruncate(shm_desc, SHM_SIZE) !=0) printf("set_up_common_memory %s\n", strerror(errno));
    shm_memory =(int*) mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc, 0);
    if (shm_memory == MAP_FAILED) printf("set_up_common_memory %s\n", strerror(errno));
    shm_memory[0] = getpid();
    shm_memory[1] = chairs;
    shm_memory[2] = -1;
    shm_memory[3] = 3;
    //0 - barber_pid;
    //1 - chairs;
    //2 - queue_end;
    //3 - waking client
    //4 - first client's pid
}

void clean_and_exit(int i){
    if (munmap(shm_memory, SHM_SIZE) == -1) printf("clean_and_exit %s\n", strerror(errno));
    if (shm_unlink(SHM_NAME)== -1) printf("clean_and_exit %s\n", strerror(errno));
    if (sem_close(sem_barber) == -1 || sem_close(sem_barber_walking)) printf("clean_and_exit %s\n", strerror(errno));
    exit(EXIT_SUCCESS);
}

void set_up_semaphores(){
    sem_barber = sem_open(SEM_BARBER, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);
    sem_barber_walking = sem_open(SEM_BARBER_WALKING, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);
    sem_barber_sleeping = sem_open(SEM_BARBER_SLEEPING, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);
    //sem_barber_cutting = sem_open(SEM_BARBER_SLEEPING, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);
    sem_waiting_room = (sem_t**)calloc(chairs, sizeof(sem_t));
    char* tmp = (char*)calloc(3, sizeof(char));

    //printf("CHA%d", chairs);
    for(int i = 0; i < chairs; i++){
      char* tmp2 = (char*)calloc(50, sizeof(char));
      sprintf(tmp, "%d", i);
      memcpy(tmp2, SEM_CUTTING, sizeof(SEM_CUTTING));
      strcat(tmp2, tmp);
      printf("%s\n", tmp2);
      sem_waiting_room[i] = sem_open(tmp2, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IWGRP, 0);
      if(sem_waiting_room[i] == SEM_FAILED){printf("ERROR sem_waiting_room %s\n", strerror(errno));}
      while(get_semaphore(sem_waiting_room[i], SEM_NOWAIT) != -1){
        printf("while\n");
      }
      free(tmp2);
    }
    fflush(stdout);
    free(tmp);
    if(sem_barber == SEM_FAILED || sem_barber_walking == SEM_FAILED) printf("set_up_semaphores %s\n", strerror(errno));
    while(get_semaphore(sem_barber, SEM_NOWAIT) != -1);
    while(get_semaphore(sem_barber_walking, SEM_NOWAIT) != -1);
    while(get_semaphore(sem_barber_sleeping, SEM_NOWAIT) != -1);
    //while(get_semaphore(sem_barber_cutting, SEM_NOWAIT) != -1);
    release_semaphore(sem_barber_sleeping);
}
/*
void hair_is_being_cut(int sig, siginfo_t *siginfo, void *context){
    barber_cut_client(siginfo->si_pid);
}

int set_up_receiving_signals(){
    signal(SIGINT, &clean_and_exit);
    struct sigaction act;
    act.sa_sigaction = &hair_is_being_cut;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
}
*/
void go_sleep(){
    get_semaphore(sem_barber_sleeping, SEM_WAIT);
    release_semaphore(sem_barber_sleeping);
    barber_cut_client(shm_memory[2]);
}
char* get_time(){
    struct timespec tp;
    if(clock_gettime(CLOCK_MONOTONIC, &tp) == -1) printf("error");
    struct tm* time = localtime(&tp.tv_sec);
    char* result = (char*)calloc(20, sizeof(char));
    sprintf(result, "%d:%d:%d:%ld", time->tm_hour, time->tm_min, time->tm_sec, tp.tv_nsec/1000);
    return result;
}

void print_info(char* info, int pid){
    char* time = get_time();
    printf("Barber - %s %d- %s\n", info, pid, time);
    free(time);
    fflush(stdout);
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

int get_next_client(){
    int result = -1;
    //3 since 0, 1, 2, 3 are occupied
    printf("AAA\n");
    fflush(stdout);
    if(shm_memory[SHM_QUEUE_END] > 3){
        result = shm_memory[4];
        for(int i = 4; i < shm_memory[3] - 1; i++){
            shm_memory[i] = shm_memory[i + 1];
        }
        shm_memory[SHM_QUEUE_END]--;
        sem_t* tmp;
        tmp = sem_waiting_room[0];
        for(int i = 0; i < chairs - 1; i++){
          sem_waiting_room[i] == sem_waiting_room[i + 1];
        }
        sem_waiting_room[chairs-1] = tmp;
        release_semaphore(tmp);
    }
    return result;
}

void barber_cut_client(int client_pid){
    print_info("start cutting client with pid: ", client_pid);
    print_info("finish cutting client with pid: ", client_pid);
    /*union sigval val;
    val.sival_int = SIGRTMIN;
    if(sigqueue(client_pid, SIGRTMIN, val) == -1){
        printf("barber_cut_client sigqueue - err %s\n", strerror(errno));
    }*/
    shm_memory[SHM_WAKING_CLIENT] = client_pid;
    release_semaphore(sem_baber_cutting);
    get_semaphore(sem_barber_walking, SEM_WAIT);
    barber_checks_waiting_room();
}

void barber_checks_waiting_room(){
    int next_client = get_next_client();
    if(next_client == -1){
        print_info("asleep", 0);
        release_semaphore(sem_barber);
        release_semaphore(sem_barber_walking);
        get_semaphore(sem_barber_sleeping, SEM_NOWAIT);
        go_sleep();
    }
    else{
        release_semaphore(sem_barber_walking);
        barber_cut_client(next_client);
    }
}
