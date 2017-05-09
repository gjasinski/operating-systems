#define _GNU_SOURCE
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>
#include "common.h"

int MSGBUF_SIZE = sizeof(struct msg_b) - sizeof(long);
int chairs;
int queue_key;
int set_id_1, set_id_2;
int set_up_queue();

void clean_and_exit(int i);

void barber_checks_waiting_room();

void set_up_semaphores();

void barber_cut_client(int client_pid);

int set_up_receiving_signals();

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Wrong arguments - barber");
        return 0;
    }
    chairs = atol(argv[1]);
    set_up_queue();
    set_up_receiving_signals();
    set_up_semaphores();
    barber_checks_waiting_room();
    while(1){}
}

key_t get_key(int key){
    char* env_home = getenv("HOME");
    return ftok(env_home, key);
}

int set_up_queue(){
    queue_key = msgget(get_key(BARBER), IPC_CREAT | S_IRUSR | S_IWUSR | S_IWGRP);
    struct msg_b buf;
    buf.mtype = 1;
    buf.chairs = chairs;
    buf.queue_end = 0;
    buf.barber_pid = getpid();
    if(msgsnd(queue_key, &buf, MSGBUF_SIZE, 0) == -1){
        printf("Couldn't setup queue\n");
        exit(-1);
    }
}

void clean_and_exit(int i){
    if(msgctl(queue_key, IPC_RMID, NULL) !=0){
        printf("clean_and_exit - msgctl - error");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void set_up_semaphores(){
    set_id_1 = semget(get_key(SEMAPHORES_1), 2, IPC_CREAT | S_IRUSR | S_IWUSR | S_IWGRP);
    set_id_2 = semget(get_key(SEMAPHORES_2), 1, IPC_CREAT | S_IRUSR | S_IWUSR | S_IWGRP);
    if (semctl(set_id_1, SEM_BARBER, SETVAL, 0) != 0) printf("2%s\n", strerror(errno));
    if (semctl(set_id_2, SEM_BARBER_WALKING, SETVAL, 0) != 0) printf("3%s\n", strerror(errno));
}

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

int get_next_client_from_fifo(int queue_key){
    struct msg_b msg;
    if(msgrcv(queue_key, &msg, MSGBUF_SIZE, 0, IPC_NOWAIT) < 0){
        printf("get_next_client_from_fifo - MSGRCV ERR");
    }
    int res;
    if(msg.queue_end > 0){
        res = msg.queue[0];
        for(int i = 0; i < msg.queue_end; i++){
            msg.queue[i] = msg.queue[i + 1];
        }
        msg.queue_end--;
    }
    else{
        res = -1;
    }
    if(msgsnd(queue_key, &msg, MSGBUF_SIZE, 0) < 0){
        printf("get_next_client_from_fifo - MSGSND ERRR");
    }
    return res;
}


void barber_cut_client(int client_pid){
    print_info("start cutting client with pid: ", client_pid);
    print_info("finish cutting client with pid: ", client_pid);
    union sigval val;
    val.sival_int = SIGRTMIN;
    if(sigqueue(client_pid, SIGRTMIN, val) == -1){
        printf("barber_cut_client sigqueue - err");
    }
    get_semaphore(set_id_2, SEM_BARBER_WALKING, 0);
    barber_checks_waiting_room();
}

void barber_checks_waiting_room(){
    int next_client = get_next_client_from_fifo(queue_key);
    if(next_client == -1){
        print_info("asleep", 0);
        release_semaphore(set_id_1, SEM_BARBER);
        release_semaphore(set_id_2, SEM_BARBER_WALKING);
    }
    else{
        release_semaphore(set_id_2, SEM_BARBER_WALKING);
        barber_cut_client(next_client);
    }
}
