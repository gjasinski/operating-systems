#define _BSD_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>

#define MAX_BUF 50

int** T;
double grid_size_x;
double grid_size_y;
int R;
int licz;

void add_to_array(char* buf);

void save_file();

void open_graph();

int main(int argc, char* argv[]) {
    licz = 0;
    if(argc != 3){
        printf("Wrong arguments - master");
        return 0;
    }
    licz = 0;
    R = atol(argv[2]);
    grid_size_x = 3.0 / R;
    grid_size_y = 2.0 / R;

    unlink(argv[1]);
    if(mkfifo(argv[1],  0777) == -1){
        printf("mkfifo - err");
        exit(0);
    }

    int fd = open(argv[1], O_RDONLY | O_NONBLOCK);

    if(fd <0){
        printf("Can't open file - master\n");
        exit(0);
    }

    T = (int**)calloc(R, sizeof(int*));
    for(int i = 0; i < R; i++){
        T[i] = (int*)calloc(R, sizeof(int));
    }

    for(int i = 0; i< R; i++){
        for(int j =0;j< R; j++){
            T[i][j] = 0;
        }
    }

    char buf[MAX_BUF];
    printf("Continue?");
    getc(stdin);
    fflush(stdout);
    int flag = 0;
    int res;
    do{
        res = (int)read(fd, buf, MAX_BUF);
        if(0 == res && flag != 0){
            break;
        }
        if(res>0) {
            flag = 1;
            add_to_array(buf);
        }
    }while(1);
    save_file();
    open_graph();
    close(fd);
}

void add_to_array(char* buf){
    int x, y, v;
    double dx, dy;
    char* str1, *str2, *str3;
    str1 = buf;
    str2 = strchr(buf, ' ') + 1;
    str3 = strchr(str2, ' ') + 1;
    for(int i =0; buf[i] != 0; i++){
        if(buf[i] == ' ') buf[i]=0;
    }
    sscanf(str1, "%lf", &dx);
    sscanf(str2, "%lf", &dy);
    v = (int)atol(str3);
    x = (int)((2 + dx)/grid_size_x);
    y = (int)((1 + dy)/grid_size_y);

    T[x][y] = v;
    if(v != 0) licz++;
}

void save_file(){
    int fd = open("data", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    char* buf = (char*)calloc(MAX_BUF, sizeof(char));
    for(int i = 0; i < R; i++){
        for(int j = 0; j < R; j++){
            sprintf(buf, "%d %d %d\n", i, j, T[i][j]);
            write(fd, buf, strlen(buf));
        }
    }
    sprintf(buf, "%c", 0);
    write(fd, buf, strlen(buf));
    close(fd);
}

void open_graph(){
    FILE *plot = popen("gnuplot", "w");
    if(!plot){
        printf("GNUPLOT ERR");
    }

    char *buf = (char *) malloc(sizeof(char) * 100);
    fputs("set view map \n", plot);
    sprintf(buf, "set xrange [0:%d]\n", R-1);
    fputs(buf, plot);
    sprintf(buf, "set yrange [0:%d]\n", R-1);
    fputs(buf, plot);
    fputs("plot 'data' with image \n", plot);
    fflush(plot);
    char q = 0;
    while(q != 'q'){
        scanf("%c", &q);
    }
    free(buf);
    fclose(plot);
}
