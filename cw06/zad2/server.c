#include "common.h"

int terminate = 0;
int connected_clients = 0;
int clients[MAX_CLIENTS][2];
mqd_t server_queue;
mqd_t create_queue(char* name);
void echo_service(char* buf);
void register_client(char* buf);
void get_server_time_service(char* buf);
void to_upper_case_service(char* buf);
void terminate_server_service(char* buf);
void terminate_queue_service(char* buf);

int main() {
    server_queue = create_queue(SERVER_NAME);
    printf("Queue created - listening\n");
    unsigned int priop;
    char buf[MQ_MSGSIZE];
    while(terminate == 0){
        ssize_t receive_size = mq_receive(server_queue, buf, MQ_MSGSIZE, &priop);
        if(receive_size > 0){
            if(buf[0] == ECHO) echo_service(buf);
            if(buf[0] == GET_SERVER_TIME) get_server_time_service(buf);
            if(buf[0] == TO_UPPER_CASE) to_upper_case_service(buf);
            if(buf[0] == TERMINATE_QUEUE) terminate_queue_service(buf);
            if(buf[0] == TERMINATE_SERVER) terminate = 1;
            if(buf[0] == HANDSHAKE) register_client(buf);
        }
    }
    terminate_server_service(buf);
    return 0;
}

mqd_t create_queue(char* name){
    struct mq_attr attr;
    attr.mq_flags = MQ_FLAGS;
    attr.mq_maxmsg = MQ_MAXMSG;
    attr.mq_msgsize = MQ_MSGSIZE;
    attr.mq_curmsgs = MQ_CURMSG;
    mode_t omask;
    omask = umask(0);
    mqd_t queue = mq_open(name, O_RDONLY | O_CREAT,(S_IRWXU | S_IRWXG | S_IRWXO ) , &attr);
    umask(omask);
    if(queue < 0){
        printf("Could't create queue - %s\n %s\n", strerror(errno), name);
        exit(-1);
    }
    return queue;
}

void register_client(char* buf){
    mqd_t queue = mq_open(buf + 1, O_RDWR);
    if(queue < 0){
        printf("Could't create queue\n");
        exit(-1);
    }

    if(connected_clients >= MAX_CLIENTS){
        printf("Can't add new client\n");
        buf[1] = -1;
    }
    else{
      buf[1] = connected_clients;
    }
    buf[2] = 0;
    clients[connected_clients][0] = queue;

    if(mq_send(queue, buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't send handshake approval\n");
        exit(-1);
    }
    if(connected_clients < MAX_CLIENTS){
      connected_clients++;
      printf("Client with id %i connected\n", connected_clients - 1);
  }
}


void echo_service(char* buf){
    if(mq_send(clients[(int)buf[1]][0], buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't resend echo\n");
        return;
    }
    printf("Server service echo - id: %d\n", (int)buf[1]);
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
    if(mq_send(clients[(int)buf[1]][0], buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't send server time\n");
        return;
    }
    printf("Server service time - id: %d\n", (int)buf[1]);
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
    if(mq_send(clients[(int)buf[1]][0], buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't send upper case str\n");
        return;
    }
    printf("Server service to upper - id: %d\n", (int)buf[1]);
}

void terminate_queue_service(char* buf){
  if(mq_send(clients[(int)buf[1]][0], buf, MQ_MSGSIZE, 0) == -1){
      printf("Couldn't terminate queue\n");
      return;
  }
  mq_close(clients[(int)buf[1]][0]);
  printf("Client with id: %d disconnected\n", (int)buf[1]);
  clients[(int)buf[1]][0] = -1;
}

void terminate_server_service(char* buf){
  buf[0] = TERMINATE_QUEUE;
  for(int i = 0; i < connected_clients; i++){
    if(clients[i][0] != -1){
      if(mq_send(clients[i][0], buf, MQ_MSGSIZE, 0) == -1){
          printf("Couldn't terminate queue\n");
      }
      printf("Client with id: %d disconnected\n", i);
      mq_close(clients[i][0]);
    }
  }
  mq_close(server_queue);
  mq_unlink(SERVER_NAME);
}
