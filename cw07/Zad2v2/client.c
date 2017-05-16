#include "common.h"
#include "string.h"

sem_t* sem_barber, * sem_barber_walking, * sem_barber_sleeping;
int cuts;
pid_t barber_pid;
int shm_desc;
int* shm_memory;
sem_t** sem_waiting_room;
sem_t* curr_sem;
char* SEM_CUTTING  = "/sem_cutting";

void get_shm();

void set_up_receiving_signals();

void go_to_barber() ;

void print_info(char* info, int pid);

int get_semaphore(sem_t* sem, int block);

void release_semaphore(sem_t* sem);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Wrong arguments - client");
        return 0;
    }
    cuts = atol(argv[1]);
    get_shm();
    //set_up_receiving_signals();
    char* tmp = (char*)calloc(3, sizeof(char));
    sem_waiting_room = (sem_t**)calloc(shm_memory[SHM_CHAIRS], sizeof(sem_t));
    for(int i = 0; i < shm_memory[SHM_CHAIRS]; i++){
      char* tmp2 = (char*)calloc(50, sizeof(char));
      sprintf(tmp, "%d", i);
      memcpy(tmp2, SEM_CUTTING, sizeof(SEM_CUTTING));
      strcat(tmp2, tmp);
      printf("%s\n", tmp2);
      sem_waiting_room[i] = sem_open(tmp2, 0);
      if(sem_waiting_room[i] == SEM_FAILED){printf("ERROR sem_waiting_room %s\n", strerror(errno));}
      while(get_semaphore(sem_waiting_room[i], SEM_NOWAIT) != -1){
        printf("while\n");
      }
      free(tmp2);
    }
    free(tmp);


    sem_barber = sem_open(SEM_BARBER, 0);
    sem_barber_walking = sem_open(SEM_BARBER_WALKING, 0);
    sem_barber_sleeping = sem_open(SEM_BARBER_SLEEPING, 0);
    if(sem_barber == SEM_FAILED || sem_barber_walking == SEM_FAILED) printf("set_up_semaphores %s\n", strerror(errno));
    get_semaphore(sem_barber_walking, SEM_WAIT);
    barber_pid = shm_memory[0];
    release_semaphore(sem_barber_walking);
    go_to_barber();
}

void get_shm(){
    shm_desc = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR | S_IWGRP);
    if (shm_desc == -1) printf("get_shm %s\n", strerror(errno));
    shm_memory =(int*) mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc, 0);
    if (shm_memory == MAP_FAILED) printf("set_up_common_memory %s\n", strerror(errno));
}

void hair_is_being_cut(int sig, siginfo_t *siginfo, void *context){
    print_info("left barber after cutting", getpid());
    cuts--;
    if(cuts == 0) exit(EXIT_SUCCESS);
  //  set_up_receiving_signals();
    go_to_barber();
}

void set_up_receiving_signals(){
    sigset_t s;
    sigemptyset(&s);
    sigaddset(&s, SIGRTMIN);
    struct sigaction act;
    act.sa_sigaction = &hair_is_being_cut;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN, &act, NULL);
    sigprocmask(SIG_UNBLOCK, &s, NULL);
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
    printf("Client - %s %d- %s\n", info, pid, time);
    free(time);
    fflush(stdout);
}

/*
 * In debugging purpose
void print_sem_value(sem_t* sem, char* sinfo){
    int info;
    sem_getvalue(sem, &info);
    printf("%s sem val %i\n", sinfo, info);
}
*/

int get_semaphore(sem_t* sem, int block){

    if (block == 1){
        if (sem_wait(sem) == -1) {
            return -1;
        }
        else{
            return 0;
        }
    }
    else{
        int tmp = sem_trywait(sem);
        return tmp;
    }
}

void release_semaphore(sem_t* sem){
    if (sem_post(sem) == -1) printf("release_semaphore %s", strerror(errno));
}


int add_client_to_shm(){
    if(shm_memory[SHM_CHAIRS] + 2 > shm_memory[SHM_QUEUE_END]){
        shm_memory[shm_memory[SHM_QUEUE_END]] = getpid();
        shm_memory[SHM_QUEUE_END]++;
        return 0;
    }
    else{
        return -1;
    }
}

void wake_up_barber(){
  shm_memory[2] = getpid();
    print_info("is waking up barber", getpid());
    release_semaphore(sem_barber_sleeping);
    //kill(barber_pid, SIGUSR1);
    get_time();
    get_time();
    print_info("left barber after cutting", getpid());
    cuts--;
}

void sit_in_waiting_room(){
    int res = add_client_to_shm();
    if (res == -1) {
        print_info("left barber - no seat in waiting room ", getpid());
        go_to_barber();
    }
    sem_t* cutting_sem = sem_waiting_room[shm_memory[SHM_QUEUE_END - 1]];

    release_semaphore(sem_barber_walking);
    print_info("take a seat in waiting room", getpid());
    get_semaphore(cutting_sem, SEM_WAIT);
    print_info("left barber after cutting", getpid());
    release_semaphore(cutting_sem);
}

void go_to_barber() {
    get_semaphore(sem_barber_walking, SEM_WAIT);
    if(get_semaphore(sem_barber, SEM_NOWAIT) == 0){
        release_semaphore(sem_barber_walking);
        wake_up_barber();
    }
    sit_in_waiting_room();
}
