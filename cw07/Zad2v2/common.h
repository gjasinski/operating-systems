#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H

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
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define QUEUE_SIZE 128
#define BARBER 1
#define SEM_BARBER "/sem_barber"
#define SEM_BARBER_WALKING "/sem_barber_walking"
#define SEM_BARBER_SLEEPING "/sem_barber_sleeping"
#define SHM_NAME "/home"
#define SHM_SIZE 128
#define SEM_NOWAIT 0
#define SEM_WAIT 1
#define SHM_BARBER_PID 0
#define SHM_CHAIRS 2
#define SHM_ARRAY_END 3

struct msg_b {
    long mtype;       /* message type, must be > 0 */;
    pid_t barber_pid;
    int chairs;
    int queue_end;
    int queue[QUEUE_SIZE];
};
#endif //ZAD1_COMMON_H
