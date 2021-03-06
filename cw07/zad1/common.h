//
// Created by gjasinski on 06.05.17.
//

#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H

#define QUEUE_SIZE 128
#define BARBER 1
#define SEMAPHORES_1 2
#define SEMAPHORES_2 3
#define SEM_BARBER 0
#define SEM_FIFO 1
#define SEM_BARBER_WALKING 0
struct msg_b {
    long mtype;       /* message type, must be > 0 */;
    pid_t barber_pid;
    int chairs;
    int queue_end;
    int queue[QUEUE_SIZE];
};
#endif //ZAD1_COMMON_H
