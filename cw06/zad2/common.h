#ifndef ZAD2_COMMON_H
#define ZAD2_COMMON_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define SERVER_NAME "/server"

#define MQ_FLAGS 0
#define MQ_MAXMSG 10
#define MQ_MSGSIZE 128
#define MQ_CURMSG 10
#define MAX_CLIENTS 10

#define HANDSHAKE 5
#define ECHO 1
#define TO_UPPER_CASE 2
#define GET_SERVER_TIME 3
#define TERMINATE_SERVER 4
#define TERMINATE_QUEUE 6

#endif //ZAD2_COMMON_H
