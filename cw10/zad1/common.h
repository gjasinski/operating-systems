#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H

#define UNIX_PATH_MAX 108
#define CLIENTS_MAX 20
#define EVENTS_MAX 20
#define NAME_SIZE_MAX 128

#define OP_ADD 1
#define OP_SUB 2
#define OP_MUL 3
#define OP_DIV 4
#define OP_RES 5
#define OP_EXIT 6
#define OP_SEND_NAME 7

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <sys/epoll.h>

#endif //ZAD1_COMMON_H
