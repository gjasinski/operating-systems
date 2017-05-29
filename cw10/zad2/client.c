#include "common.h"
int socket_desc;
char* unix_name;

void local_loop(char** argv);

void net_loop(char** argv);

void clear_and_exit(int n);

void receive_compute_send_loop(char** argv);


int main (int argc, char** argv)
{
    if(argc != 3 && argc != 4 && sizeof(argv[2]) < NAME_SIZE_MAX - 3){
        printf("./main [name] [IPv4 address] [port]\n./main [name] [UNIX socket path]\n");
        exit(-1);
    }
    unix_name = (char*)calloc(strlen(argv[1]), sizeof(char));
    strcpy(unix_name, argv[1]);
    signal(SIGINT, &clear_and_exit);
    if(argc == 3){
        local_loop(argv);
    }
    else{
        net_loop(argv);
    }
    return 0;
}

void local_loop(char** argv){
    socket_desc = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(socket_desc == -1) {
        printf("create socket error %s\n", strerror(errno));
        exit(-1);
    }
    unlink(unix_name);
    struct sockaddr_un unix_addr;
    unix_addr.sun_family = AF_UNIX;
    memcpy(unix_addr.sun_path, argv[1], sizeof(argv[1]));

    if(bind(socket_desc, (struct sockaddr *)&unix_addr, sizeof(unix_addr)) == -1){
        printf("bind socket error %s\n", strerror(errno));
        exit(-1);
    }

    memcpy(unix_addr.sun_path, argv[2], sizeof(argv[2]));
    if(connect(socket_desc, (struct sockaddr *)&unix_addr, sizeof(unix_addr)) == -1){
        printf("connect socket error %s\n", strerror(errno));
        exit(-1);
    }
    receive_compute_send_loop(argv);
}

void net_loop(char** argv){
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_desc == -1) {
        printf("create socket error %s\n", strerror(errno));
        exit(-1);
    }

    int port = atoi(argv[3]);
    struct sockaddr_in inet_addr;
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = port;

    inet_pton(AF_INET, argv[2], &inet_addr.sin_addr);

    if(connect(socket_desc, (struct sockaddr *)&inet_addr, sizeof(inet_addr)) == -1){
        printf("connect socket error %s\n", strerror(errno));
        exit(-1);
    }
    receive_compute_send_loop(argv);
}

void receive_compute_send_loop(char** argv){
    long int result;
    char* buf = (char*)calloc(NAME_SIZE_MAX, sizeof(char));
    char sign[5];
    sign[1] = '+';
    sign[2] = '-';
    sign[3] = '*';
    sign[4] = '/';
    sprintf(buf, "%c%c%s", OP_SEND_NAME, (int)sizeof(argv[1]), argv[1]);
    fflush(stdout);
    if(write(socket_desc, (void *)buf, strlen(argv[1]) + 2) == -1) {
        printf("Sending name err - %s\n", strerror(errno));
    }
    fflush(stdout);
    while(1){
        free(buf);
        buf = (char*)calloc(NAME_SIZE_MAX, sizeof(char));
        if(read(socket_desc, (void*)buf, NAME_SIZE_MAX) == -1){
            printf("Receive msg err - %s\n", strerror(errno));
        }
        if(buf[0] == OP_EXIT) {
            printf("[EXIT] Terminating client\n");
            break;
        }
        if(buf[0] == OP_REJECT){
            printf("[REJECT] This name is occupied\n");
            break;
        }
        if(buf[0] == OP_PING){
            printf("[PING]\n");
            write(socket_desc, (void*)buf, 1);
            continue;
        }
        //0 - operation// 1 - id // 2 - len1 // 3 - len2
        int a = atoi(buf + 4);
        int b = atoi(buf + 5 + buf[2]);
        result = -1;
        if(buf[0] == OP_ADD) result = a + b;
        if(buf[0] == OP_SUB) result = a - b;
        if(buf[0] == OP_MUL) result = a * b;
        if(buf[0] == OP_DIV) {
            if(b == 0){
                printf("[%d] FLOATING POINT EXCEPTION\n", buf[1]);
                buf[0] = OP_FPE;
                buf[2] = 0;
                write(socket_desc, (void*)buf, 3);
                continue;
            }
            result = a / b;
        }
        if(result == -1) {
            printf("[UNKNOWN] Client error unknown msg_id - %d - ignoring\n", buf[0]);
            continue;
        }
        else{
            printf("[%d] %d %c %d = %ld\n", buf[1], a, sign[buf[0]], b, result);
        }
        buf[0] = OP_RES;
        sprintf(buf + 3, "%ld", result);
        buf[2] = strlen(buf + 3);
        if(write(socket_desc, (void*)buf, strlen(buf + 3) + 3) == -1){
            printf("Send msg err - %s\n", strerror(errno));
        }
    }
    free(buf);
    if(shutdown(socket_desc, SHUT_RDWR) == -1){
        printf("Shutdown err - %s\n", strerror(errno));
    }
    if(close(socket_desc) == -1){
        printf("Close err - %s\n", strerror(errno));
    }
}
void clear_and_exit(int n){
    char buf[2];
    buf[0] = OP_EXIT;
    buf[1] = 0;
    if(write(socket_desc, (void*)buf, 2) == -1){
        printf("Send exit_info err - %s\n", strerror(errno));
    }
    unlink(unix_name);
    free(unix_name);
    if(shutdown(socket_desc, SHUT_RDWR) == -1){
        printf("Shutdown err - %s\n", strerror(errno));
    }
    if(close(socket_desc) == -1){
        printf("Close err - %s\n", strerror(errno));
    }
    exit(0);
}