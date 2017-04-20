#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "common.h"

pid_t listener_pid;
int client_id;

mqd_t open_server_queue();
mqd_t create_client_queue(char *env_home);
void connect_to_server(int server_queue, int client_queue, char* buf, char* path);
void clean();
void listen(int client_queue);

int main() {
    char* buf = (char*)calloc(MQ_CURMSG, sizeof(char));
    char* path;
    mqd_t server_queue = open_server_queue();
    mqd_t client_queue = create_client_queue(path);
    connect_to_server(server_queue, client_queue, buf, path);

    listener_pid = fork();
    atexit(clean);

    if(listener_pid == 0) listen(client_queue);
    char c;
    size_t size = MQ_MSGSIZE;
    do{
        buf[1] = client_id;
        buf[2] = 0;
        scanf("%c", &c);
        if(c == '1' || c == '2'){
            getline(&buf, &size, stdin);
        } else{
            buf[0] = 0;
        }
        if(c == '1') {
            buf[0] = ECHO;
            if(mq_send(server_queue, buf, MQ_MSGSIZE, 0) == -1){
                printf("Couldn't send ECHO request");
            }
        }
        if(c == '2') {
            buf[0] = TO_UPPER_CASE;
            if(mq_send(server_queue, buf, MQ_MSGSIZE, 0) == -1){
                printf("Couldn't send TO_UPPER_CASE request");
            }
        }
        if(c == '3') {
            buf[0] = GET_SERVER_TIME;
            if(mq_send(server_queue, buf, MQ_MSGSIZE, 0) == -1){
                printf("Couldn't send GET_SERVER_TIME request");
            }
        }
        if(c == '4') {
            buf[0] = TERMINATE_SERVER;
            if(mq_send(server_queue, buf, MQ_MSGSIZE, 0) == -1){
                printf("Couldn't send TERMINATE_SERVER request");
            }
        }
    }while(c != '4' && c != 'q');
    free(buf);
}

void clean(){}

mqd_t open_server_queue(){
    char *env_home;
    srand(time(NULL));
    env_home = getenv("HOME");
    strcat(env_home, SERVER_NAME);
    return mq_open(env_home, O_RDWR);
}

mqd_t create_client_queue(char *env_home){
    env_home = getenv("HOME");
    char name[5];
    name[4] = 0;
    for(int i = 0; i < 4; i++){
        name[i] = 97 + rand() % 32;
    }
    strcat(env_home, name);
    struct mq_attr attr;
    attr.mq_flags = MQ_FLAGS;
    attr.mq_maxmsg = MQ_MAXMSG;
    attr.mq_msgsize = MQ_MSGSIZE;
    attr.mq_curmsgs = MQ_CURMSG;
    mqd_t queue = mq_open(name, O_RDWR | O_CREAT, 0777, &attr);
    if(queue < 0){
        printf("Could't create queue");
        exit(-1);
    }
    return queue;
}

void connect_to_server(int server_queue, int client_queue, char* buf, char* path){
    buf[0] = HANDSHAKE;
    strcpy(buf + 1, path);
    if(mq_send(server_queue, buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't send handshake approval");
        exit(-1);
    }
    sleep(1);
    unsigned int priop;
    if(mq_receive(client_queue, buf, MQ_MSGSIZE, &priop) == -1){
        printf("Could't receive handshake");
        exit(-1);
    }
    client_id = (int)buf[1];
}

void listen(int client_queue){
    char buf[MQ_MSGSIZE];
    unsigned int priop;
    while(1){
        sleep(1);
        printf("-\n");
        ssize_t receive_size = mq_receive(client_queue, buf, MQ_MSGSIZE, &priop);
        if(receive_size > 0){
            printf(buf + 1);
        }
    }
}
