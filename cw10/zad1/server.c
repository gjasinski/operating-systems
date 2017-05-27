#include "common.h"

int desc_unix_socket, desc_inet_socket, inet_connected_clients, unix_connected_clients;
pthread_t thread_listener, thread_pinger;
struct sockaddr_in* inet_clients;
struct sockaddr_un* unix_clients;
socklen_t inet_len = sizeof(struct sockaddr_in);
socklen_t unix_len = sizeof(struct sockaddr_un);

void clear_and_exit(int n);

void* listen_socket(void* useless);

void* ping_clients(void* useless);

int main (int argc, char** argv)
{
    if(argc != 3){
        printf("./main port socket_path");
        exit(-1);
    }
    signal(SIGINT, &clear_and_exit);
    int port = atoi(argv[1]);
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

    if(pthread_create(&thread_listener, NULL, &listen_socket, NULL) != 0 ||
            pthread_create(&thread_pinger, NULL, &ping_clients, NULL) != 0){
        printf("create thread error%s\n", strerror(errno));
        exit(-1);
    }
    int a, b,c;
    /*struct sockaddr_in tmp;
    socklen_t len = sizeof(tmp);
    if(getsockname(desc_inet_socket, (struct sockaddr*)&tmp, &len) == -1)printf("err");
    printf("port: %d add: %zu\n", tmp.sin_port, tmp.sin_addr);*/
    while(1){
        printf("Type: 0 - +\n 1 - -\n 2 - *\n 3 - /\n");
        char operation;
        if(operation == 'q') break;
        scanf("%d %d %d", &a, &b, &c);

        printf("%d %d %d", a, b, c);
    }

}

void* listen_socket(void* useless){
    printf("unix: %d inet %d\n", desc_unix_socket, desc_inet_socket);
    struct epoll_event* events = calloc(CLIENTS_MAX, sizeof(struct epoll_event));
    struct epoll_event event;
    int incoming_fd;
    inet_connected_clients = unix_connected_clients = 0;
    inet_clients = (struct sockaddr_in*)calloc(CLIENTS_MAX, sizeof(struct sockaddr_in));
    unix_clients = (struct sockaddr_un*)calloc(CLIENTS_MAX, sizeof(struct sockaddr_un));
    if(listen(desc_unix_socket, CLIENTS_MAX) != 0 || listen(desc_inet_socket, EVENTS_MAX) != 0){
        printf("open listening on threads%s\n", strerror(errno));
        exit(-1);
    }

    int epoll_desc = epoll_create(1);
    event.data.fd = desc_inet_socket;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    if (epoll_ctl(epoll_desc, EPOLL_CTL_ADD, desc_inet_socket, &event) == -1) {
        printf("epoll_ctl err%s\n", strerror(errno));
    }
    event.data.fd = desc_unix_socket;
    if (epoll_ctl(epoll_desc, EPOLL_CTL_ADD, desc_unix_socket, &event) == -1) {
        printf("epoll_ctl err%s\n", strerror(errno));
    }
    char* buf = (char*)calloc(NAME_SIZE_MAX, sizeof(char));
    while(1) {
        int n = epoll_wait(epoll_desc, events, EVENTS_MAX, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].events & EPOLLRDHUP) {
                printf("epoll error\n");
                continue;
            }
            if (events[i].data.fd == desc_inet_socket) {
                while (1) {
                    incoming_fd = accept(desc_inet_socket, (struct sockaddr *)
                            (inet_clients + inet_connected_clients * sizeof(struct sockaddr_in)), &inet_len);
                    if (incoming_fd == -1) break;
                    inet_connected_clients++;
                    event.data.fd = incoming_fd;
                    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                    if (epoll_ctl(epoll_desc, EPOLL_CTL_ADD, incoming_fd, &event) == -1) {
                        printf("epoll_ctl err%s\n", strerror(errno));
                    }
                    printf("conntected inet\n");
                }
            } else if (events[i].data.fd == desc_unix_socket) {
                while (1) {
                    incoming_fd = accept(desc_unix_socket, (struct sockaddr *)
                            (unix_clients + unix_connected_clients * sizeof(struct sockaddr_un)), &unix_len);
                    if (incoming_fd == -1) break;
                    unix_connected_clients += sizeof(struct sockaddr_un);
                    event.data.fd = incoming_fd;
                    event.events = EPOLLIN | EPOLLOUT | EPOLLET;

                    if (epoll_ctl(epoll_desc, EPOLL_CTL_ADD, incoming_fd, &event) == -1) {
                        printf("epoll_ctl err%s\n", strerror(errno));
                    }
                    printf("conntected unix\n");
                }
            } else {
                while(1) {
                    struct sockaddr tmp;
                    socklen_t tmplen = sizeof(tmp);
                    //int bytes_read = (int) recvfrom(events[i].data.fd, buf, NAME_SIZE_MAX, 0, &tmp, &tmplen);
                    int bytes_read = (int)read(events[i].data.fd, buf, NAME_SIZE_MAX);
                    if (bytes_read == 0) {printf("AAAA");break;}

                    printf("%dodebralem wiadomosc %s od %d\n", bytes_read, buf + 2, sizeof(buf+2));
                    printf("%dodebralem wiadomosc %s od %d\n", bytes_read, buf + 12, sizeof(buf+2));
                }
            }
        }
        if(n == -1)break;//temporary, to calm clion ;)
    }
    return NULL;
}

void* ping_clients(void* useless){

    return NULL;
}

void clear_and_exit(int n){
    printf("exit_function_create it!");
    exit(0);
}