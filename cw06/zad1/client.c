#include "common.h"
#define MAX_CLIENTS 10

int clients[MAX_CLIENTS][2];
int connected_clients;
int MSGBUF_SIZE  = sizeof(struct msg_b) - sizeof(long);
int client_id;
int client_queue;
pid_t listener_pid;

void remove_queue(key_t key);
void send_handshake(int queue_server, int key);
void receive_handshake(int queue_server);
void listen(int client_queue, pid_t msg_id);
void send_msg(int queue_server, int mtype, char *mtext);
void clean();

int main(){
    pid_t client_pid = getpid();
    char* env_home;
    env_home = getenv("HOME");
    key_t key_server = ftok(env_home, SERVER);
    key_t key_client = ftok(env_home, (char)getpid());
    int server_queue = msgget(key_server, 0);
    client_queue = msgget(key_client, IPC_CREAT | S_IRUSR | S_IWUSR | S_IWGRP);

    if(server_queue < 0 || client_queue < 0){
        printf("msgget - error %s\n", strerror(errno));
        exit(-1);
    }

    send_handshake(server_queue, key_client);
    receive_handshake(client_queue);
    fflush(stdout);
    struct msg_b msg;
    char c;
    listener_pid = fork();
    atexit(clean);
    if(listener_pid == 0) listen(client_queue, client_pid);
    char* buf = (char*)calloc(MSGBUF_SIZE, sizeof(char));
    size_t size = MSGBUF_SIZE;
    do{
        scanf("%c", &c);
        if(c == '1' || c == '2'){
            getline(&buf, &size, stdin);
        } else{
            buf[0] = 0;
        }
        if(c == '1') send_msg(server_queue, ECHO, buf);
        if(c == '2') send_msg(server_queue, TO_UPPER_CASE, buf);
        if(c == '3') send_msg(server_queue, GET_SERVER_TIME, buf);
        if(c == '4') send_msg(server_queue, TERMINATE_SERVER, buf);
    }while(c != '4' && c != 'q');

    free(buf);
}

void clean(){
    kill(listener_pid, SIGKILL);
    remove_queue(client_queue);
    exit(0);
}
void send_msg(int queue_server, int mtype, char *mtext){
    struct msg_b msg;
    msg.mtype = mtype;
    msg.pid = getpid();
    strcpy(msg.mtext, mtext);
    if(msgsnd(queue_server, &msg, MSGBUF_SIZE, 0) == -1){
        printf("Couldn't send_msg %s\n", strerror(errno));
    }
}

void send_handshake(int queue_server, int key){
    char buf[MSGBUF_SIZE];
    sprintf(buf, "%d", key);
    send_msg(queue_server, HANDSHAKE, buf);
}

void receive_handshake(int queue_server){
    struct msg_b msg;
    sleep(1);
    int res = msgrcv(queue_server, &msg, MSGBUF_SIZE, 0, IPC_NOWAIT);
    if(res >= 0) {
        printf("Handshake received from server id: %s\n", msg.mtext);
        client_id = atol(msg.mtext);
    }
    else {
        printf("Could't receive handshake %s\n", strerror(errno));
        exit(-1);
    }
}

void remove_queue(key_t key){
    if(msgctl(key, IPC_RMID, NULL) !=0){
        printf("msgctl - error\n");
        exit(-1);
    }
}

void listen(int client_queue, pid_t msg_id){
    struct msg_b msg;
    while(1){
        sleep(1);
        int res = msgrcv(client_queue, &msg, MSGBUF_SIZE, 0, IPC_NOWAIT);
        if(res >= 0){
            if(msg.mtype == msg_id) printf("Client: %s\n", msg.mtext);
        }
    }
}
