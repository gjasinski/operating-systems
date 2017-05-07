//
// Created by gjasinski on 06.05.17.
//

#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H

#define QUEUE_SIZE 128
#define BARBER 1
#define SEMAPHORES 2
#define SEM_BARBER 0
#define SEM_FIFO 1
struct msg_b {
    long mtype;       /* message type, must be > 0 */;
    int chairs;
    int queue_end;
    int queue[QUEUE_SIZE];
};
#endif //ZAD1_COMMON_H
