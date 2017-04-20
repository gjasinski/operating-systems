#include "common.h"
#define MAX_CLIENTS 10

int clients[MAX_CLIENTS][2];
int connected_clients;
int MSGBUF_SIZE  = sizeof(struct msg_b) - sizeof(long);
int terminate_client = 0;

void register_client(struct msg_b buf);
void echo_service(struct msg_b buf);
void to_upper_case_service(struct msg_b buf);
void get_server_time_service(struct msg_b buf);
void terminate_server_service(struct msg_b buf);
void terminate_queue_service(struct msg_b buf);
void remove_queue(key_t key);

int main() {
    char* env_home;
    env_home = getenv("HOME");
    key_t key_server = ftok(env_home, SERVER);
    int server_queue = msgget(key_server, IPC_CREAT | S_IRUSR | S_IWUSR | S_IWGRP);

    if(server_queue < 0){
        printf("msgget - error\n");
        exit(-1);
    }

    struct msg_b msg;
    printf("Queue created - listening\n");
    while(terminate_client == 0){
        sleep(1);
        int res = msgrcv(server_queue, &msg, MSGBUF_SIZE, 0, IPC_NOWAIT);
        if(res >= 0){
            printf("message: %s\n", msg.mtext);
            if(msg.mtype == HANDSHAKE) register_client(msg);
            if(msg.mtype == ECHO) echo_service(msg);
            if(msg.mtype == TO_UPPER_CASE) to_upper_case_service(msg);
            if(msg.mtype == GET_SERVER_TIME) get_server_time_service(msg);
            if(msg.mtype == TERMINATE_SERVER) terminate_server_service(msg);
            if(msg.mtype == TERMINATE_QUEUE) terminate_queue_service(msg);
        }
    }
    remove_queue(server_queue);
}

void remove_queue(key_t key){
    if(msgctl(key, IPC_RMID, NULL) !=0){
        printf("msgctl - error");
        exit(-1);
    }
    exit(0);
}

void register_client(struct msg_b buf){
    if(connected_clients >= MAX_CLIENTS){
        printf("Can't add new client");
        return;
    }
    key_t key = atol(buf.mtext);
    int res = msgget(key, 0);
    if(res == -1){
        printf("register_client - msgget - error");
        return;
    }
    clients[connected_clients][0] = buf.pid;
    clients[connected_clients][1] = res;
    sprintf(buf.mtext, "%d", connected_clients);

    buf.mtype = buf.pid;
    buf.pid = getpid();
    if(msgsnd(clients[connected_clients][1], &buf, MSGBUF_SIZE, 0) == -1){
        printf("Couldn't send_msg echo\n");
        return;
    }
    connected_clients++;
    printf("Client with pid %d registered\n", (int)buf.mtype);
}

int find_client(int pid){
    int queue_id = -1;
    for(int i = 0; i < connected_clients; i++){
        if(clients[i][0] == pid){
            queue_id = clients[i][1];
            break;
        }
    }
    return queue_id;
}

void remove_client(int pid){
  for(int i = 0; i < connected_clients; i++){
      if(clients[i][0] == pid){
          clients[i][0] = -1;
          return;
      }
  }
}

void echo_service(struct msg_b buf){
    int queue_id = find_client(buf.pid);
    if(queue_id == -1){
        printf("Client's queue not found\n");
        return;
    }
    buf.mtype = buf.pid;
    buf.pid = getpid();
    if(msgsnd(queue_id, &buf, MSGBUF_SIZE, 0) == -1){
        printf("Couldn't send_msg echo\n");
        return;
    }
    printf("Server service echo - pid: %d\n", (int)buf.mtype);
}

char* get_time(){
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    return asctime (timeinfo);
}

void get_server_time_service(struct msg_b buf){
    int queue_id = find_client(buf.pid);
    if(queue_id == -1){
        printf("Client's queue not found");
        return;
    }
    buf.mtype = buf.pid;
    buf.pid = getpid();
    buf.mtext[0] = 0;
    strcpy(buf.mtext, get_time());
    if(msgsnd(queue_id, &buf, MSGBUF_SIZE, 0) == -1){
        printf("Couldn't send_msg echo");
        return;
    }
    printf("Server service time - pid: %d\n", (int)buf.mtype);
}

void to_upper(char* string){
    for(int i = 0; string[i] != 0; i++){
        if(string[i] >= 97 && string [i] < 123){
            string[i] = string[i] - 32;
        }
    }
}

void to_upper_case_service(struct msg_b buf){
    int queue_id = find_client(buf.pid);
    if(queue_id == -1){
        printf("Client's queue not found");
        return;
    }
    buf.mtype = buf.pid;
    buf.pid = getpid();
    to_upper(buf.mtext);
    if(msgsnd(queue_id, &buf, MSGBUF_SIZE, 0) == -1){
        printf("Couldn't send_msg echo");
        return;
    }
    printf("Server service to upper - pid: %d\n", (int)buf.mtype);
}

void terminate_server_service(struct msg_b buf){
    terminate_client = 1;
    for(int i = 0; i < connected_clients; i++){
        if(clients[i][0] > 0){
          buf.mtype = TERMINATE_QUEUE;
          buf.pid = getpid();
          if(msgsnd(clients[i][1], &buf, MSGBUF_SIZE, 0) == -1){
              printf("Couldn't termiante_queue\n");
              return;
          }
          printf("Server terminated queue - pid: %d\n", (int)clients[i][0]);
        }
    }
    printf("Server service terminate\n");
}

void terminate_queue_service(struct msg_b buf){
  int queue_id = find_client(buf.pid);
  remove_client(buf.pid);
  if(queue_id == -1){
      printf("Client's queue not found\n");
      return;
  }
  buf.mtype = TERMINATE_QUEUE;
  int pid = buf.pid;
  buf.pid = getpid();
  if(msgsnd(queue_id, &buf, MSGBUF_SIZE, 0) == -1){
      printf("Couldn't termiante_queue\n");
      return;
  }
  printf("Server terminated queue - pid: %d\n", pid);
}
