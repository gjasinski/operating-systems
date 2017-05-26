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
#define SEM_QUEUE "/sem_queue"
#define SEM_BARBER_SLEEPING "/sem_barber_sleeping"
#define SEM_CUTTING "sem_cutting"
#define SHM_NAME_QUEUE "/homeq"
#define SHM_NAME_SEMAPHORES "/homes"
#define SHM_SIZE 128
#define SEM_NOWAIT 0
#define SEM_WAIT 1
#define SHM_BARBER_PID 0
#define SHM_CHAIRS 1
#define SHM_WAKING_CLIENT 2
#define SHM_QUEUE_END 3

#endif //ZAD1_COMMON_H