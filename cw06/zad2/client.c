#include "common.h"

pid_t listener_pid;
int client_id;

mqd_t open_server_queue();
mqd_t create_client_queue(char *env_home);
void connect_to_server(int server_queue, int client_queue, char* buf, char* client_queue_name);
void listen(int client_queue, int server_queue, char* client_queue_name);

int main() {
    char client_queue_name[5];
    char* buf = (char*)calloc(MQ_CURMSG, sizeof(char));
    char* buf_input = (char*)calloc(MQ_CURMSG, sizeof(char));
    mqd_t server_queue = mq_open(SERVER_NAME, O_RDWR);
    mqd_t client_queue = create_client_queue(client_queue_name);
    connect_to_server(server_queue, client_queue, buf, client_queue_name);

    listener_pid = fork();
    if(listener_pid == 0) listen(client_queue, server_queue, client_queue_name);
    char c;
    size_t size = MQ_MSGSIZE;
    do{
        buf[1] = client_id;
        buf[2] = 0;
        scanf("%c", &c);
        if(c == '1' || c == '2'){
            getline(&buf_input, &size, stdin);
            strcpy(buf + 2, buf_input);
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
        if(c == 'q'){
          buf[0] = TERMINATE_QUEUE;
          if(mq_send(server_queue, buf, MQ_MSGSIZE, 0) == -1){
              printf("Couldn't send TERMINATE_QUEUE request");
          }
        }
    }while(c != '4' && c != 'q');
    free(buf);
    free(buf_input);
    while(1){};
}

mqd_t create_client_queue(char *name){
    srand(time(NULL));
    name[4] = 0;
    name[0] = '/';
    for(int i = 1; i < 4; i++){
        name[i] = 97 + rand() % 25;
    }
    struct mq_attr attr;
    attr.mq_flags = MQ_FLAGS;
    attr.mq_maxmsg = MQ_MAXMSG;
    attr.mq_msgsize = MQ_MSGSIZE;
    attr.mq_curmsgs = MQ_CURMSG;
    mqd_t queue = mq_open(name, O_RDWR | O_CREAT, 0777, &attr);
    if(queue < 0){
      printf("%s\n", name);
        printf("Could't create queue\n");
        exit(-1);
    }
    return queue;
}

void connect_to_server(int server_queue, int client_queue, char* buf, char* path){
    buf[0] = HANDSHAKE;
    strcpy(buf + 1, path);
    if(mq_send(server_queue, buf, MQ_MSGSIZE, 0) == -1){
        printf("Couldn't send handshake approval\n");
        exit(-1);
    }
    unsigned int priop;
    if(mq_receive(client_queue, buf, MQ_MSGSIZE, &priop) == -1){
        printf("Could't receive handshake\n");
        exit(-1);
    }
    if(buf[1] == -1){
      printf("Could't receive handshake\n");
      exit(-1);
    }
    client_id = buf[1];
    printf("Connected to server with id: %d\n", client_id);
}

void listen(int client_queue, int server_queue, char* client_queue_name){
    char buf[MQ_MSGSIZE];
    unsigned int priop;
    while(1){
        ssize_t receive_size = mq_receive(client_queue, buf, MQ_MSGSIZE, &priop);
        if(receive_size > 0){
            if(buf[0] == TERMINATE_QUEUE){
              mq_close(server_queue);
              mq_close(client_queue);
              mq_unlink(client_queue_name);
              kill(getppid(), SIGKILL);
              exit(0);
            }
            printf("Client: %s\n", buf + 2);
        }
    }
}
