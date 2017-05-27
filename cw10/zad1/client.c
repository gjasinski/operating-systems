#include "common.h"
int socket_desc;

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
    socket_desc = socket(AF_UNIX, SOCK_STREAM, 0);
    if(socket_desc == -1) {
        printf("create socket error %s\n", strerror(errno));
        exit(-1);
    }
    struct sockaddr_un unix_addr;
    unix_addr.sun_family = AF_UNIX;
    memcpy(unix_addr.sun_path, argv[2], sizeof(argv[2]));

    if(connect(socket_desc, (struct sockaddr *)&unix_addr, sizeof(unix_addr)) == -1){
        printf("connect socket error %s\n", strerror(errno));
        exit(-1);
    }
    receive_compute_send_loop(argv);
}

void net_loop(char** argv){
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1) {
        printf("create socket error %s\n", strerror(errno));
        exit(-1);
    }

    int port = atoi(argv[3]);
    uint32_t address = (uint32_t)atol(argv[2]);

    struct in_addr sin_addr;
    sin_addr.s_addr = htonl(address);
    struct sockaddr_in inet_addr;
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = port;
    inet_addr.sin_addr = sin_addr;

    if(connect(socket_desc, (struct sockaddr *)&inet_addr, sizeof(inet_addr)) == -1){
        printf("connect socket error %s\n", strerror(errno));
        exit(-1);
    }
    getc(stdin);
    printf("ALA MA KOTA");
    fflush(stdout);
    receive_compute_send_loop(argv);
}

void receive_compute_send_loop(char** argv){
    long int result;
    char* buf = (char*)calloc(NAME_SIZE_MAX, sizeof(char));
    buf[0] = OP_SEND_NAME;
    buf[1] = sizeof(argv[1]);
    sprintf(buf+2, "%s", argv[1]);

    if(write(socket_desc, (void *)buf, sizeof(buf)) == -1){
    //printf("%d\n", (int)write(socket_desc, (void *)buf, strlen(argv[1]) + 2));/*
        printf("Sending name err - %s\n", strerror(errno));
    }

    while(1){
        /*int received_bytes = */read(socket_desc, buf, NAME_SIZE_MAX);
        /*if(received_bytes == -1 || received_bytes - 1 != buf[1] + buf[2]){
            printf("Receive msg err - %s\n", strerror(errno));
        }*/
        if(buf[0] == OP_EXIT) break;
        if(buf[0] == OP_REJECT){
            printf("This name is occupied\n");
            break;
        }
        if(buf[0] == OP_PING){
            write(socket_desc, buf, 2);
            continue;
        }
        //0 - operation 1 - id // 2 - len1 // 3 - len2
        int a = atoi(buf + 4);
        int b = atoi(buf + 5 + buf[2]);
        result = -1;
        if(buf[0] == OP_ADD) result = a + b;
        if(buf[0] == OP_SUB) result = a - b;
        if(buf[0] == OP_MUL) result = a * b;
        if(buf[0] == OP_DIV) result = a / b;
        if(result == -1) {
            printf("Client error unknown msg_id - %d - ignoring\n", buf[0]);
            continue;
        }
        buf[0] = OP_RES;
        sprintf(buf + 3, "%ld", result);
        buf[2] = strlen(buf + 3);
        if(write(socket_desc, buf, sizeof(buf)) == -1){
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
    if(shutdown(socket_desc, SHUT_RDWR) == -1){
        printf("Shutdown err - %s\n", strerror(errno));
    }
    if(close(socket_desc) == -1){
        printf("Close err - %s\n", strerror(errno));
    }
    exit(0);
}
