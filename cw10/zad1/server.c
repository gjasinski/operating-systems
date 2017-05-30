#include "common.h"

int desc_unix_socket, desc_inet_socket, inet_connected_clients, unix_connected_clients, connected_clients;
pthread_t thread_listener, thread_pinger;
struct sockaddr_in* inet_clients;
struct sockaddr_un* unix_clients;
socklen_t inet_len = sizeof(struct sockaddr_in);
socklen_t unix_len = sizeof(struct sockaddr_un);
char** clients_list;
int* clients_mask, * clients_ping;
int epoll_desc;
struct epoll_event* events;
struct epoll_event event;
pthread_mutex_t synchronise_ping_and_receive;
char* unix_name;

void clear_and_exit(int n);

void* listen_socket(void* useless);

void* ping_clients(void* useless);

int main (int argc, char** argv)
{
    if(argc != 3){
        printf("./main port socket_path");
        exit(-1);
    }
    unix_name = (char*)calloc(strlen(argv[2]), sizeof(char));
    strcpy(unix_name, argv[2]);
    signal(SIGINT, &clear_and_exit);
    srand(time(NULL));
    int port = atoi(argv[1]);
    synchronise_ping_and_receive = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    connected_clients = 0;
    desc_unix_socket = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
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
    clients_mask = (int*)calloc(CLIENTS_MAX, sizeof(int));
    clients_ping = (int*)calloc(CLIENTS_MAX, sizeof(int));
    for(int i = 0; i < CLIENTS_MAX; i++){
        clients_mask[i] = 0;
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
            while(clients_mask[j] == 0){
                j++;
                j = j % CLIENTS_MAX;
            }
        }
        sprintf(tmp1, "%d", a);
        sprintf(tmp2, "%d", c);
        sprintf(buf, "%c%c%c%c%d%c%d", b, operation %128, (int)strlen(tmp1), (int)strlen(tmp2), a, 0, c);
        if(write(clients_mask[j], (void*)buf, strlen(tmp1) + strlen(tmp2) + 5) == -1){
            printf("Send request err - %s\n", strerror(errno));
        };
        printf("[REQUEST SERVICE %d] %d %c %d for %s\n", operation, a, sign[b], c, clients_list[j]);
        operation++;
    }

}

void* listen_socket(void* useless){
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    char* buf = (char*)calloc(NAME_SIZE_MAX, sizeof(char));
    int incoming_fd;
    if(listen(desc_unix_socket, CLIENTS_MAX) != 0 || listen(desc_inet_socket, EVENTS_MAX) != 0){
        printf("open listening on threads%s\n", strerror(errno));
        exit(-1);
    }
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
        //pthread_mutex_unlock(&synchronise_ping_and_receive);
        int n = epoll_wait(epoll_desc, events, EVENTS_MAX, -1);
        //pthread_mutex_lock(&synchronise_ping_and_receive);
        for (int i = 0; i < n; i++) {
            if (events[i].events & EPOLLRDHUP) {
                printf("epoll error\n");
                continue;
            }
            //add client from net
            if (events[i].data.fd == desc_inet_socket) {
                while (1) {
                    incoming_fd = accept(desc_inet_socket, (struct sockaddr *)
                            (inet_clients + inet_connected_clients * sizeof(struct sockaddr_in)), &inet_len);
                    if (incoming_fd == -1) break;
                    inet_connected_clients++;
                    event.data.fd = incoming_fd;
                    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                    int flags = fcntl(incoming_fd, F_GETFL,0);
                    flags |= O_NONBLOCK;
                    fcntl(incoming_fd, F_SETFL, flags);
                    if (epoll_ctl(epoll_desc, EPOLL_CTL_ADD, incoming_fd, &event) == -1) {
                        printf("epoll_ctl err%s\n", strerror(errno));
                    }
                }
            }
                //add local client
            else if (events[i].data.fd == desc_unix_socket) {
                while (1) {
                    incoming_fd = accept(desc_unix_socket, (struct sockaddr *)
                            (unix_clients + unix_connected_clients * sizeof(struct sockaddr_un)), &unix_len);
                    if (incoming_fd == -1) break;
                    unix_connected_clients += 1;
                    event.data.fd = incoming_fd;
                    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                    int flags = fcntl(incoming_fd, F_GETFL,0);
                    flags |= O_NONBLOCK;
                    fcntl(incoming_fd, F_SETFL, flags);
                    if (epoll_ctl(epoll_desc, EPOLL_CTL_ADD, incoming_fd, &event) == -1) {
                        printf("epoll_ctl err%s\n", strerror(errno));
                    }
                }
            }
                //handle received message
            else {
                free(buf);
                buf = (char*)calloc(NAME_SIZE_MAX, sizeof(char));
                if(read(events[i].data.fd, buf, NAME_SIZE_MAX) == 0) continue;
                //Register client - check if name is not used
                if(buf[0] == OP_SEND_NAME){
                    int name_taken = 0;
                    int free_slot = -1;
                    for(int j = 0; j < CLIENTS_MAX && name_taken == 0; j++){
                        if(clients_mask[j] == 0){
                            free_slot = j;
                            continue;
                        }
                        if(strcmp(buf + 2, clients_list[j]) == 0){
                            name_taken = 1;
                        }
                    }
                    if(name_taken == 1 || free_slot == -1){
                        buf[0] = OP_REJECT;
                        buf[1] = 0;
                        if(write(events[i].data.fd, (void *)buf, 2) == -1){
                            printf("Write error %s\n", strerror(errno));
                        }
                        printf("[REJECT REGISTER] %s %d\n", buf + 2, events[i].data.fd);
                        epoll_ctl(epoll_desc, EPOLL_CTL_DEL, events[i].data.fd, &event);
                    }
                    else{
                        clients_mask[free_slot] = events[i].data.fd;
                        sprintf(clients_list[free_slot], "%s", buf + 2);
                        printf("[REGISTER] %s %d\n", buf + 2, events[i].data.fd);
                        connected_clients++;
                    }
                    continue;
                }

                //Print computed result
                if(buf[0] > 0 && buf[0] < 6){
                    printf("[%d] %s\n", buf[1], buf + 3);
                    continue;
                }

                //Remove client
                if(buf[0] == OP_EXIT){
                    int id_to_remove = 0;
                    while(clients_mask[id_to_remove] != events[i].data.fd){
                        id_to_remove++;
                    }
                    printf("[REMOVE] %s %d\n", clients_list[id_to_remove], events[i].data.fd);
                    epoll_ctl(epoll_desc, EPOLL_CTL_DEL, events[i].data.fd, &event);
                    clients_mask[id_to_remove] = 0;
                    connected_clients--;
                    continue;
                }

                if(buf[0] == OP_PING){
                    for(int j = 0; j < CLIENTS_MAX; j++){
                        if(clients_mask[j] == events[i].data.fd){
                            clients_ping[j] = 0;
                            break;
                        }
                    }
                    continue;
                }
                if(buf[0] == OP_FPE){
                    printf("[%d] DO NOT DIVIDE BY ZERO\n", buf[1]);
                    continue;
                }
            }
        }
        if(n == -1)break;//temporary, to calm clion ;)
    }
    return NULL;
}

void* ping_clients(void* useless){
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    char buf[2];
    buf[0] = OP_PING;
    buf[1] = 0;
    while(1){
        //pthread_mutex_lock(&synchronise_ping_and_receive);
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
    return NULL;
}

void clear_and_exit(int n){
    pthread_cancel(thread_pinger);
    pthread_cancel(thread_listener);
    char buf[2];
    buf[0] = OP_EXIT;
    buf[1] = 0;
    for(int i = 0; i < CLIENTS_MAX; i++){
        if(clients_mask[i] == 0) continue;
        write(clients_mask[i], (void*)buf, 2);
        free(clients_list[i]);
    }
    free(clients_list);
    free(clients_mask);
    shutdown(desc_inet_socket, SHUT_RDWR);
    shutdown(desc_unix_socket, SHUT_RDWR);
    close(desc_inet_socket);
    close(desc_unix_socket);
    pthread_join(thread_listener, NULL);
    pthread_join(thread_pinger, NULL);
    unlink(unix_name);
    free(unix_name);
    exit(0);
}
