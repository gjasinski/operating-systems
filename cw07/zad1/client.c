#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include "common.h"

int MSGBUF_SIZE = sizeof(struct msg_b) - sizeof(long);
int cuts;
int queue_key;
int semaphore_set;
pid_t barber_pid;

int get_queue();

int set_up_receiving_signals();

key_t get_key(int key);

void go_to_barber() ;

void release_semaphore(int semaphore_set, int semaphore_no);

int get_semaphore(int semaphore_set, int semaphore_no, short flag);

void print_info(char* info, int pid);

void get_barber_pid();

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Wrong arguments - client");
        return 0;
    }
    cuts = atol(argv[1]);
    queue_key = get_queue();
    set_up_receiving_signals();
    semaphore_set = semget(get_key(SEMAPHORES), 0, 0);
    get_barber_pid();
    go_to_barber();
}

key_t get_key(int key){
    char* env_home = getenv("HOME");
    return ftok(env_home, key);
}

int get_queue(){
    struct msg_b buf;
    int queue = msgget(get_key(BARBER), 0);
    if(msgsnd(queue, &buf, MSGBUF_SIZE, 0) == -1){
        printf("Couldn't get queue\n");
        exit(-1);
    }
    return queue;
}

void get_barber_pid(){
    struct msg_b msg;
    get_semaphore(semaphore_set, SEM_FIFO, 0);
    msgrcv(queue_key, &msg, MSGBUF_SIZE, 0, IPC_NOWAIT);
    msgsnd(queue_key, &msg, MSGBUF_SIZE, 0);
    release_semaphore(semaphore_set, SEM_FIFO);
    barber_pid = msg.barber_pid;
}

void hair_is_being_cut(int sig, siginfo_t *siginfo, void *context){
    print_info("left barber after cutting", getpid());
    cuts--;
    if(cuts == 0) exit(EXIT_SUCCESS);
    go_to_barber();
}

int set_up_receiving_signals(){
    struct sigaction act;
    act.sa_sigaction = &hair_is_being_cut;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN, &act, NULL);
}

char* get_time(){
    struct timespec tp;
    if(clock_gettime(CLOCK_MONOTONIC, &tp) == -1) printf("error");
    struct tm* time = localtime(&tp.tv_sec);
    char* result = (char*)calloc(20, sizeof(char));
    sprintf(result, "%d:%d:%d:%ld", time->tm_hour, time->tm_min, time->tm_sec, tp.tv_nsec);
    return result;
}

void print_info(char* info, int pid){
    char* time = get_time();
    printf("Client - %s %d- %s\n", info, pid, time);
    free(time);
    fflush(stdout);
}

int get_semaphore(int semaphore_set, int semaphore_no, short flag){
    struct sembuf sops;
    sops.sem_num = semaphore_no;
    sops.sem_flg = flag;
    sops.sem_op = -1;
    return semop(semaphore_set, &sops, 1);
}

void release_semaphore(int semaphore_set, int semaphore_no){
    struct sembuf sops;
    sops.sem_num = semaphore_no;
    sops.sem_flg = 0;
    sops.sem_op = 1;
    semop(semaphore_set, &sops, 1);
}


int add_client_to_fifo(){
    struct msg_b msg;
    msgrcv(queue_key, &msg, MSGBUF_SIZE, 0, IPC_NOWAIT);
    if(msg.queue_end < msg.chairs){
        msg.queue[msg.queue_end] = getpid();
        msg.queue_end++;
        msgsnd(queue_key, &msg, MSGBUF_SIZE, 0);
        return 0;
    }
    else{
        msgsnd(queue_key, &msg, MSGBUF_SIZE, 0);
        return -1;
    }
}

void wake_up_barber(){
    kill(barber_pid, SIGUSR1);
//    release_semaphore(semaphore_set, SEM_BARBER_SLEEPING);
    while(1){}
}

void sit_in_waiting_room(){
    int res = add_client_to_fifo();
    release_semaphore(semaphore_set, SEM_FIFO);
    if (res == -1) {
        print_info("left barber - no seat in waiting room ", getpid());
        go_to_barber();
    }
    while(1){};
}

void go_to_barber() {
    if(get_semaphore(semaphore_set, SEM_BARBER, IPC_NOWAIT) == 0){
        wake_up_barber();
    }
    get_semaphore(semaphore_set, SEM_FIFO, 0);
    sit_in_waiting_room();
}

