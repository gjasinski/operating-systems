#include "common.h"

int desc_unix_socket, desc_inet_socket, inet_connected_clients, unix_connected_clients, connected_clients;
pthread_t thread_listener, thread_pinger;
struct sockaddr_in* inet_clients;
struct sockaddr_un* unix_clients;
socklen_t inet_len = sizeof(struct sockaddr_in);
socklen_t unix_len = sizeof(struct sockaddr_un);
char** clients_list;
int* clients_ping, * clients_mask;
unsigned int *clients_addrlen;
int epoll_desc;
struct epoll_event* events;
struct epoll_event event;
char* unix_name;
struct sockaddr** clients_dest_addr;

void clear_and_exit(int i);

void* listen_socket(void* useless);

void* ping_clients(void* useless);

int main (int argc, char** argv){
    if(argc != 3){
        printf("./main port socket_path");
        exit(-1);
    }
    unix_name = (char*)calloc(strlen(argv[2]), sizeof(char));
    strcpy(unix_name, argv[2]);
    signal(SIGINT, &clear_and_exit);
    srand(time(NULL));
    int port = atoi(argv[1]);
    connected_clients = 0;
    desc_unix_socket = socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    desc_inet_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(desc_inet_socket == -1 || desc_unix_socket == -1) {
        printf("create socket error %s\n", strerror(errno));
        exit(-1);
    }
    struct in_addr sin_addr;
    sin_addr.s_addr = htonl(INADDR_ANY);
    struct sockaddr_in inet_addr;
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = port;
    inet_addr.sin_addr = sin_addr;

    struct sockaddr_un unix_addr;
    unix_addr.sun_family = AF_UNIX;
    memcpy(unix_addr.sun_path, argv[2], sizeof(argv[2]));
    unlink(argv[2]);

    if(bind(desc_inet_socket, (struct sockaddr *)&inet_addr, sizeof(inet_addr)) == -1 ||
       bind(desc_unix_socket, (struct sockaddr *)&unix_addr, sizeof(unix_addr)) == -1) {
        printf("bind socket error %s\n", strerror(errno));
        exit(-1);
    }
    int flags = fcntl(desc_unix_socket, F_GETFL,0);
    flags |= O_NONBLOCK;
    fcntl(desc_unix_socket, F_SETFL, flags);
    flags = fcntl(desc_inet_socket, F_GETFL,0);
    flags |= O_NONBLOCK;
    fcntl(desc_inet_socket, F_SETFL, flags);

    events = calloc(CLIENTS_MAX, sizeof(struct epoll_event));
    clients_list = (char**)calloc(CLIENTS_MAX, sizeof(char*));
    clients_ping = (int*)calloc(CLIENTS_MAX, sizeof(int));
    clients_dest_addr = (struct sockaddr**)calloc(CLIENTS_MAX, sizeof(struct sockaddr));
    clients_addrlen = (unsigned int *)calloc(CLIENTS_MAX, sizeof(unsigned int));
    clients_mask = (int*)calloc(CLIENTS_MAX, sizeof(int));
    for(int i = 0; i < CLIENTS_MAX; i++){
        clients_addrlen[i] = clients_mask[i] = 0;
        clients_list[i] = (char*)calloc(NAME_SIZE_MAX, sizeof(char));
    }
    inet_connected_clients = unix_connected_clients = 0;
    inet_clients = (struct sockaddr_in*)calloc(CLIENTS_MAX, sizeof(struct sockaddr_in));
    unix_clients = (struct sockaddr_un*)calloc(CLIENTS_MAX, sizeof(struct sockaddr_un));

    if(pthread_create(&thread_listener, NULL, &listen_socket, NULL) != 0 ||
       pthread_create(&thread_pinger, NULL, &ping_clients, NULL) != 0) {
        printf("create thread error%s\n", strerror(errno));
        exit(-1);
    }
    int a, b,c;

    char* buf = (char*)calloc(NAME_SIZE_MAX, sizeof(char));
    int operation = 0;
    char tmp1[NAME_SIZE_MAX/2];
    char tmp2[NAME_SIZE_MAX/2];
    char sign[5];
    sign[1] = '+';
    sign[2] = '-';
    sign[3] = '*';
    sign[4] = '/';
    printf("Type: a op b\n where op:\n 1 - +\n 2 - -\n 3 - *\n 4 - /\n");
    int j = 0;
    struct sockaddr* address;
    while(1){
        if(operation == 'q') break;
        scanf("%d %d %d", &a, &b, &c);
        if(connected_clients == 0){
            printf("[INFO] 0 connected clients\n");
            continue;
        }
        int n = rand()%CLIENTS_MAX;
        for(int i = 0; i < n/2 + 1; i++){
            j = (j+1)%CLIENTS_MAX;
            while(clients_addrlen[j] == 0){
                j++;
                j = j % CLIENTS_MAX;
            }
        }
        sprintf(tmp1, "%d", a);
        sprintf(tmp2, "%d", c);
        sprintf(buf, "%c%c%c%c%d%c%d", b, operation %128, (int)strlen(tmp1), (int)strlen(tmp2), a, 0, c);

        if(sendto(clients_mask[j], (void*)buf, strlen(tmp1) + strlen(tmp2) + 5, 0,
                  clients_dest_addr[j], clients_addrlen[j]) == -1){
            printf("Send request err - %s\n", strerror(errno));
        };
        printf("[REQUEST SERVICE %d] %d %c %d for %s\n", operation, a, sign[b], c, clients_list[j]);
        operation++;
    }

}

void* listen_socket(void* useless){
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    char* buf = (char*)calloc(NAME_SIZE_MAX, sizeof(char));

    epoll_desc = epoll_create(1);
    event.data.fd = desc_inet_socket;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    if (epoll_ctl(epoll_desc, EPOLL_CTL_ADD, desc_inet_socket, &event) == -1) {
        printf("epoll_ctl err%s\n", strerror(errno));
    }
    event.data.fd = desc_unix_socket;
    if (epoll_ctl(epoll_desc, EPOLL_CTL_ADD, desc_unix_socket, &event) == -1) {
        printf("epoll_ctl err%s\n", strerror(errno));
    }

    while(1) {
        struct sockaddr* src_addr = (struct sockaddr*)calloc(1,sizeof(struct sockaddr_in));
        socklen_t addrlen = sizeof(struct sockaddr_in);
        int n = epoll_wait(epoll_desc, events, EVENTS_MAX, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].events & EPOLLRDHUP){
                printf("epoll error\n");
                continue;
            }
            free(buf);
            buf = (char *)calloc(NAME_SIZE_MAX, sizeof(char));
            if (recvfrom(events[i].data.fd, buf, NAME_SIZE_MAX, 0, src_addr, &addrlen) == 0) continue;
            //Register client - check if name is not used
            if (buf[0] == OP_SEND_NAME) {
                int name_taken = 0;
                int free_slot = -1;
                for (int j = 0; j < CLIENTS_MAX && name_taken == 0; j++) {
                    if (clients_addrlen[j] == 0) {
                        free_slot = j;
                        continue;
                    }
                    if (strcmp(buf + 2, clients_list[j]) == 0) {
                        name_taken = 1;
                    }
                }
                if (name_taken == 1 || free_slot == -1) {
                    buf[0] = OP_REJECT;
                    buf[1] = 0;
                    if (write(events[i].data.fd, (void *) buf, 2) == -1) {
                        printf("Write error %s\n", strerror(errno));
                    }
                    printf("[REJECT REGISTER] %s %d\n", buf + 2, events[i].data.fd);
                    epoll_ctl(epoll_desc, EPOLL_CTL_DEL, events[i].data.fd, &event);
                } else {
                    sprintf(clients_list[free_slot], "%s", buf + 2);
                    clients_mask[free_slot] = events[i].data.fd;
                    clients_dest_addr[free_slot] = src_addr;
                    clients_addrlen[free_slot] = addrlen;
                    printf("[REGISTER] %s %d\n", buf + 2, events[i].data.fd);
                    connected_clients++;
                }
                continue;
            }

            //Print computed result
            if (buf[0] > 0 && buf[0] < 6) {
                printf("[%d] %s\n", buf[1], buf + 3);
                continue;
            }

            //Remove client
            if (buf[0] == OP_EXIT) {
                int id_to_remove = 0;
                while (clients_mask[id_to_remove] != events[i].data.fd) {
                    id_to_remove++;
                }
                printf("[REMOVE] %s %d\n", clients_list[id_to_remove], events[i].data.fd);
                clients_mask[id_to_remove] = 0;
                clients_addrlen[id_to_remove] = 0;
                clients_ping[id_to_remove] = 0;
                free(clients_dest_addr[id_to_remove]);
                connected_clients--;
                continue;
            }

            if (buf[0] == OP_PING) {
                for (int j = 0; j < CLIENTS_MAX; j++) {
                    if (clients_mask[j] == events[i].data.fd) {
                        clients_ping[j] = 0;
                        break;
                    }
                }
                continue;
            }
            if (buf[0] == OP_FPE) {
                printf("[%d] DO NOT DIVIDE BY ZERO\n", buf[1]);
                continue;
            }

        }
        if(n == -1)break;//temporary, to calm clion ;)
    }
    return NULL;
}

void* ping_clients(void* useless){
   /* pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    char buf[2];
    buf[0] = OP_PING;
    buf[1] = 0;
    while(1){
        for(int i = 0; i < CLIENTS_MAX; i++){
            if(clients_mask[i] == 0) continue;
            clients_ping[i] = 1;
            if(write(clients_mask[i], (void *)buf, 1) == 0){
                printf("[PING REMOVE ] %s %d\n", clients_list[i], clients_mask[i]);
                epoll_ctl(epoll_desc, EPOLL_CTL_DEL, clients_mask[i], &event);
                clients_mask[i] = 0;
                clients_list[i][0] = 0;
                clients_ping[i] = 0;
            }
            sleep(1);
        }
        for(int i = 0; i < CLIENTS_MAX; i++){
            if(clients_mask[i] != 0 && clients_ping[i] == 1){
                printf("[PING REMOVE] %s %d\n", clients_list[i], clients_mask[i]);
                epoll_ctl(epoll_desc, EPOLL_CTL_DEL, clients_mask[i], &event);
                clients_mask[i] = 0;
                clients_list[i][0] = 0;
                clients_ping[i] = 0;
                connected_clients--;
            }
        }

    }
    return NULL;*/
}

void clear_and_exit(int n){
    pthread_cancel(thread_pinger);
    pthread_cancel(thread_listener);
    char buf[2];
    buf[0] = OP_EXIT;
    buf[1] = 0;
    for(int i = 0; i < CLIENTS_MAX; i++){
        if(clients_mask[i] == 0) continue;
        sendto(clients_mask[i], (void*)buf, 2, 0, clients_dest_addr[i], clients_addrlen[i]);
        free(clients_list[i]);
        free(clients_dest_addr[i]);
    }
    free(clients_list);
    free(clients_mask);
    free(clients_dest_addr);
    free(clients_addrlen);
    free(clients_ping);
    close(desc_inet_socket);
    close(desc_unix_socket);
    pthread_join(thread_listener, NULL);
    pthread_join(thread_pinger, NULL);
    unlink(unix_name);
    free(unix_name);
    exit(0);
}