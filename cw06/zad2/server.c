#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>


#include "common.h"
int terminate = 0;
int connected_clients = 0;
int clients[MAX_CLIENTS][2];
mqd_t create_queue(char* name);
void echo_service(char* buf);
void register_client(char* buf);
void get_server_time_service(char* buf);
void to_upper_case_service(char* buf);

int main() {
    char *env_home;
    env_home = getenv("HOME");
    strcat(env_home, SERVER_NAME);
    mqd_t server_queue = create_queue(env_home);
    printf("Queue created - listening\n");
    unsigned int priop;
    char buf[MQ_MSGSIZE];
    while(terminate == 0){
        sleep(1);
        printf("-\n");
        ssize_t receive_size = mq_receive(server_queue, buf, MQ_MSGSIZE, &priop);
        if(receive_size > 0){
            printf(buf);
            if(buf[0] == ECHO) echo_service(buf);
            if(buf[0] == GET_SERVER_TIME) get_server_time_service(buf);
            if(buf[0] == TO_UPPER_CASE) to_upper_case_service(buf);
            //if(buf[0] == ) echo_service(buf);
            if(buf[0] == HANDSHAKE) register_client(buf);
        }
    }

}

mqd_t create_queue(char* name){
    struct mq_attr attr;
    attr.mq_flags = MQ_FLAGS;
    attr.mq_maxmsg = MQ_MAXMSG;
    attr.mq_msgsize = MQ_MSGSIZE;
    //attr.mq_curmsgs = MQ_CURMSG;
    mqd_t queue = mq_open(name, O_RDWR | O_CREAT | O_NONBLOCK, 0664, &attr);
    if(queue < 0){
        printf("Could't create queue - %s\n %s\n", strerror(errno), name);
        exit(-1);
    }
    return queue;
}

void register_client(char* buf){
    if(connected_clients >= MAX_CLIENTS){
        printf("Can't add new client");
        return;
    }
    mqd_t queue = mq_open(buf + 1, O_RDWR);
    if(queue < 0){
        printf("Could't create queue");
        exit(-1);
    }

    clients[connected_clients][0] = queue;
    buf[1] = connected_clients;
    buf[2] = 0;
    if(mq_send(queue, buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't send handshake approval");
        exit(-1);
    }

    connected_clients++;
    printf("Client with id %i registered\n", connected_clients - 1);
}


void echo_service(char* buf){
    if(mq_send(clients[buf[1]][0], buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't resend echo");
        return;
    }
    printf("Server service echo - id: %d\n", clients[buf[1]][0]);
}

char* get_time(){
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    return asctime (timeinfo);
}

void get_server_time_service(char* buf){
    strcpy(buf + 2, get_time());
    if(mq_send(clients[buf[1]][0], buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't send server time");
        return;
    }
    printf("Server service time - id: %d\n", buf[1]);
}

void to_upper(char* string){
    for(int i = 0; string[i] != 0; i++){
        if(string[i] >= 97 && string [i] < 123){
            string[i] = string[i] - 32;
        }
    }
}

void to_upper_case_service(char* buf){
    to_upper(buf + 2);
    if(mq_send(clients[buf[1]][0], buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't send upper case str");
        return;
    }
    printf("Server service to upper - id: %d\n", buf[1]);
}
