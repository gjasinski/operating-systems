#ifndef ZAD2_COMMON_H
#define ZAD2_COMMON_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <memory.h>
#include <errno.h>
#include <signal.h>

#define SERVER 0
#define CLIENT 1
#define STATEMENT_LENGTH 50
#define HANDSHAKE 5
#define ECHO 1
#define TO_UPPER_CASE 2
#define GET_SERVER_TIME 3
#define TERMINATE_SERVER 4

struct msg_b {
    long mtype;       /* message type, must be > 0 */
    pid_t pid;
    char mtext[STATEMENT_LENGTH];    /* message data */
};

#endif //ZAD2_COMMON_H
